#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <mpi.h>
#include <mkl.h>
#include <omp.h>
#include "global.h"

_OFFLOADABLE static double my_deltaphi;
_OFFLOADABLE static double* svec;
void precompute_gamma_3d() {

	int lsize = get_lmax()+1;
	double* restrict cl = get_cl_array();
	double* restrict noise = get_noise_array();
	double* restrict beam = get_beam_array();

	svec = (double*) malloc(lsize*sizeof(double));
	if (!svec) printf("ERROR: Couldn't malloc svec.\n");
	int l;
	const double third = 1.0/3.0;
	for (l = 0; l < lsize; l++) {
		svec[l] = pow(2.0*l+1.0,third)*(cl[l]+noise[l]/(beam[l]*beam[l]));
	}

	my_deltaphi = deltaphi;
	#pragma offload_transfer target(mic) in(svec[0:lsize] : ALLOC RETAIN) \
                                            in(my_deltaphi)
}

// original l1 upper bound
#ifndef L1_CUTOFF
#define L1_CUTOFF (lmax+1)
#endif

_OFFLOADABLE void decompose_gamma_3d_mpi(thread_g3d_data_t* thread_data, int rank, int numranks, double workshare) 
{
    int i, j, k;
    int lmax = get_lmax();
    int lsize = lmax+1;

    // Step 1) Count the number of iterations in the original loop.
    int count = 0;
    for (i = 2; i < L1_CUTOFF; i++) {
        for (j = i; j < lsize; j++) {
            for (k = j + (i % 2); k < min(i+j,lmax)+1; k += 2) {
                count++;
            }
        }
    }

    // Step 2) Divvy up the total iterations between the MPI ranks
    // number of iterations for this task
    int blk_size_raw = count / numranks;
    int left_over = count - (blk_size_raw * numranks);

    // Step 3) Find the start and end points of the iterations for this MPI rank
    int r;
    int i0, j0, k0;
    int i1, j1, k1;
    int dummy;

    i0 = 2;
    j0 = i0;
    k0 = j0;
    // iterate through the preceding ranks up to and excluding this rank
    for (r=0; r < rank; r++)
    {
        int rankwork = blk_size_raw;
        if (r < left_over) rankwork += 1;

        get_next_ijk_bound(i0, j0, k0, &i1, &j1, &k1, L1_CUTOFF, lsize, lsize, &dummy, rankwork);
        i0 = i1;
        j0 = j1;
        k0 = k1;
    }

    // get the bounds for this MPI rank
    // give raw amount of work to every worker in every task
    int work = 0;
    work += blk_size_raw;

    if (rank < left_over)
        work += 1;

    // Get bounds for the host
    int host_work = (int) ( (1.0 - workshare)*work );
    get_next_ijk_bound(i0, j0, k0, &i1, &j1, &k1, L1_CUTOFF, lsize, lsize, &dummy, host_work);

    // if on device then get the bounds and work count for the device
    #ifdef __MIC__
    i0 = i1;
    j0 = j1;
    k0 = k1;
    int mic_work = work - host_work;
    get_next_ijk_bound(i0, j0, k0, &i1, &j1, &k1, L1_CUTOFF, lsize, lsize, &dummy, mic_work);
    work = mic_work;

    #else
    work = host_work;
    #endif

    // Step 4) Divvy up the iterations for this MPI rank between the OMP threads
    int nthreads = omp_get_num_threads();

    // hack. host has no nested parallelism
    const int ntasks = nthreads;

    int thrd_work = 0;
    int rank_count = work;

    int rank_blk_size_raw = rank_count / ntasks;
    int rank_left_over = rank_count - (rank_blk_size_raw * ntasks);

    // give raw amount of work to every worker in every task
    thrd_work += rank_blk_size_raw;

    // go through all the threads and assign them their upper and lower bounds
    int final_i = i1;
    int final_j = j1;
    int final_k = k1;

    int t;
    for (t=0; t < nthreads; t++)
    {
        thrd_work = rank_blk_size_raw;
        int taskid = t;

        if (taskid < rank_left_over) thrd_work += 1;
        get_next_ijk_bound(i0, j0, k0, &i1, &j1, &k1, final_i, final_j, final_k, &dummy, thrd_work);
        //printf("tid=%d: -- start = (%d, %d, %d), end = (%d, %d, %d), work=%d\n", t, i0, j0, k0, i1, j1, k1, thrd_work);

        thread_data[t].i0 = i0;
        thread_data[t].j0 = j0;
        thread_data[t].k0 = k0;
        thread_data[t].i1 = i1;
        thread_data[t].j1 = j1;
        thread_data[t].k1 = k1;
        i0 = i1;
        j0 = j1;
        k0 = k1;
    }
}

_OFFLOADABLE void get_next_ijk_bound(int i0, int j0, int k0, int *i1, int *j1, int *k1, \
        int final_i, int final_j, int final_k, int* blocksize, int maxblocksize)
{
    int lmax = get_lmax();
    int lsize = lmax+1;
    int i,j,k;
    int _blocksize=0;

    int iout, jout, kout;
    iout = final_i;
    jout = final_j;
    kout = final_k;

    int start_i = i0;
    int end_i = final_i + 1;
    for (i=start_i; i<end_i; i++)
    {
        int start_j = i == i0 ? j0 : i;
        int end_j = i == final_i ? final_j+1 : lsize;
        for (j=start_j; j<end_j; j++)
        {
            int start_k = j+(i%2);
            int end_k = min(i+j,lmax)+1;
            if ((i==i0) && (j==j0))           
            {
                start_k = k0;
            }
            if ((i==final_i) && (j==final_j)) 
            {
                end_k = final_k;
            }

            for (k = start_k; k < end_k; k += 2) 
            {
                if (_blocksize == maxblocksize)
                {
                    iout = i;
                    jout = j;
                    kout = k;
                    goto end;
                }
                _blocksize++;
            }
        }
    }
end:
    // cap the output at the final value
    if ( (iout>=final_i) && (jout>=final_j) && (kout>final_k) )
    {
        iout = final_i;
        jout = final_j;
        kout = final_k;
    }

    // output the last i,j,k values and the block size
    *i1 = iout;
    *j1 = jout;
    *k1 = kout;
    *blocksize = _blocksize;
    return;
}

_OFFLOADABLE void calculate_gamma_3d(int n, thread_g3d_data_t* thread_data)
{
	int i,j,k,m,s,t1,t2,t3;
	int terms = get_terms_prim();
	int terms_pad = (terms + 7) & ~7;
	int lmax = get_lmax();
	int lsize = lmax+1;
	int pmax_late = get_pmax_late();

    // get the thread, task and worker ids
	int tid = omp_get_thread_num();
	int nthreads = omp_get_num_threads();
    const int ntasks = nthreads;
	int nworkers = nthreads/ntasks;

    int xsize = thread_data[tid].xsize;
    double* xvec = thread_data[tid].xvec;
    double* yvec = thread_data[tid].yvec;
    double* mvec = thread_data[tid].mvec;
    double* xdiff = thread_data[tid].xdiff;
    double* ixdiff = thread_data[tid].ixdiff;
    double* x = thread_data[tid].intgrlvec;
    double xmax = xvec[xsize-1];

    double* restrict basis_late_flat = get_basis_late_array();
    double (*restrict basis_late)[pmax_late+1] = (double (*restrict)[pmax_late+1]) basis_late_flat;

    // store the loop bounds
    int i0 = thread_data[tid].i0;
    int j0 = thread_data[tid].j0;
    int k0 = thread_data[tid].k0;
    int final_i = thread_data[tid].i1;
    int final_j = thread_data[tid].j1;
    int final_k = thread_data[tid].k1;

    int i1, j1, k1;
    int dummy;
    int maxblocksize = 1<<31;
    get_next_ijk_bound(i0,j0,k0,&i1,&j1,&k1,final_i,final_j,final_k,&dummy,maxblocksize);

    int start_i = i0;
    int end_i = i1+1;

    for (i=start_i; i<end_i; i++)
    {
        int start_j = i == i0 ? j0 : i;
        int end_j = i == i1 ? j1+1 : lsize;
        for (j=start_j; j<end_j; j++)
        {
            int start_k = j+(i%2);
            int end_k = min(i+j,lmax)+1;
            if ((i==i0) && (j==j0))
            {
                start_k = k0;
            }
            if ((i==i1) && (j==j1))
            {
                end_k = k1;
            }

            for (k = start_k; k < end_k; k += 2) 
            {
                double x = calculate_xint(i,j,k,n,xsize,xvec,yvec,thread_data[tid].task_spl);

                double s1 = svec[i];
                double s2 = svec[j];
                double s3 = svec[k];
                double z = permsix(i,j,k)*calculate_geometric(i,j,k)/sqrt(s1*s2*s3);


                for (m = 0; m < terms; m++)
                {
                    int p1,p2,p3;
                    find_perm_late(m,&p1,&p2,&p3);

                    double b1,b2,b3,b4,b5,b6;
                    b1 = basis_late[i][p1]*basis_late[j][p2]*basis_late[k][p3];
                    b2 = basis_late[i][p2]*basis_late[j][p3]*basis_late[k][p1];
                    b3 = basis_late[i][p3]*basis_late[j][p1]*basis_late[k][p2];
                    b4 = basis_late[i][p3]*basis_late[j][p2]*basis_late[k][p1];
                    b5 = basis_late[i][p2]*basis_late[j][p1]*basis_late[k][p3];
                    b6 = basis_late[i][p1]*basis_late[j][p3]*basis_late[k][p2];

                    double y = (b1+b2+b3+b4+b5+b6)/(6.0);
                    mvec[m] += x * y * z;
                }
            }
        }
    }

    for (m = 0; m < terms; m++)
    {
        mvec[m] *= 6.0*my_deltaphi*my_deltaphi;
    }
}