      subroutine shado_xch_TB(arx_in,nx_loc,ny_loc,nz_loc,
     &   myrankx,myranky,myrankz,nprocx,nprocy,nprocz)
!======================================================================
!  Routine to do top-bottom (or up-down) shadow-zone exchange.
!
!  If this process has a neighbour above or below, a shadow-zone
!  swap will occur.
!
!  The topmost surface of arr_in (i.e., the physical local domain)
!  is sent to the bottom-most surface of arx_in (the extended local 
!  domain) of the process above - if there is one.
!  The bottom-most surface of arr_in (i.e., the physical local domain)
!  is sent to the topmost surface of arx_in (i.e., the extended  
!  local domain) of the process below - if there is one.
!
!  Similarly, the bottom-most surface of arr_in (the physical local
!  domain) from any process above is received into the topmost
!  surface of arx_in (i.e., the extended local domain). 
!  The topmost surface of arr_in (i.e., the physical local domain)
!  from any process below is received into the bottom-most surface
!  of arx_in (i.e., the extended local domain).
!  
!  To help avoid contention, let even processes send upwards first,
!  while odd processes send downwards first.  That way, all processes
!  should have data ready to receive when MPI_Recv is called.
!
!  If at topmost or bottommost boundaries of the global domain, just 
!  copy the edge of arr_in into the edge of arx_in. 
!======================================================================
!
      implicit none
      integer myrankx,myranky,myrankz,nprocx,nprocy,nprocz,myrank
      integer nx_loc, ny_loc, nz_loc, i, j, k
      real arx_in(nx_loc+2,ny_loc+2,nz_loc+2)
      real shado_T(nx_loc,ny_loc)       !  For data moving upwards
      real shado_B(nx_loc,ny_loc)       !   "    "   "     downwards
      logical ltop,lbottom
!
      include 'mpif.h'
      integer istatus(MPI_STATUS_SIZE),ierror,idestt,idestb
      integer, parameter :: itagt=627, itagb=726
!
! 
! First, bottom global boundary treatment:
!
      if(myrankz.eq.0) then     ! Am at the bottom boundary already
!$OMP PARALLEL DO PRIVATE(i,j)
        do j=2,ny_loc+1         ! Just copy south sfc of arr_in to arx_in
          do i=2,nx_loc+1
            arx_in(i,j,1) = arx_in(i,j,2)  ! simplifies filter 
          enddo
        enddo
!$OMP END PARALLEL DO
      endif
!
!
! Next, top global boundary treatment:
!
      if(myrankz.eq.nprocz-1) then   ! Am at the top boundary already
!$OMP PARALLEL DO PRIVATE(i,j)
        do j=2,ny_loc+1         ! Just copy top sfc of arr_in to arx_in
          do i=2,nx_loc+1
            arx_in(i,j,nz_loc+2) = arx_in(i,j,nz_loc+1)  ! simplifes filter 
          enddo
        enddo
!$OMP END PARALLEL DO
      endif
!
!
! For max. efficiency, let even procs send up first, then down, while
!                           odd procs send down first, then up.
!
      ltop = .false.
      lbottom = .false.
      if( mod(myrankz,2).eq.1 ) go to 102
!
! Top local boundary treatment:
!
  101 continue
      if(myrankz.lt.nprocz-1) then   ! need to do a shadow-exchange
!$OMP PARALLEL DO PRIVATE(i,j)
        do j=2,ny_loc+1              ! Send above, receive from above
          do i=2,nx_loc+1
            shado_T(i-1,j-1) = arx_in(i,j,nz_loc+1)
          enddo
        enddo
!$OMP END PARALLEL DO
!
! Send/recv to/from process "idestt", with same myrankx,y and myrankz+1:
          idestt= (myrankz+1)*nprocx*nprocy + myranky*nprocx + myrankx
!     
!========================================================================
!  Note: it makes sense to post the "receive" here first, since data from
!  the "odd" procs is already "sent".  Otherwise, if everything is "sent"
!  before anything is "received", there could be too much data
!  simultaneously in flight for some MPI implementations...
!========================================================================
!
          call MPI_Recv(shado_B(1,1),nx_loc*ny_loc,MPI_REAL,
     &                  idestt,itagb,MPI_COMM_WORLD,istatus,ierror)
!
          call MPI_Send(shado_T(1,1),nx_loc*ny_loc,MPI_REAL,
     &                  idestt,itagt,MPI_COMM_WORLD,ierror)
!
!$OMP PARALLEL DO PRIVATE(i,j)
          do j=2,ny_loc+1
            do i=2,nx_loc+1
              arx_in(i,j,nz_loc+2) = shado_B(i-1,j-1)
            enddo
          enddo
!$OMP END PARALLEL DO
!
      endif
!debug       write(9+myrank,*) 'Shado TB, finished send/recv above'
      ltop = .true. 
      if(lbottom) go to 103
!
!
! Bottom local boundary treatment:
!
  102 continue
      if(myrankz.gt.0) then   ! need to do a shadow-exchange
!$OMP PARALLEL DO PRIVATE(i,j)
        do j=2,ny_loc+1       ! Send below, receive from below
          do i=2,nx_loc+1
            shado_B(i-1,j-1) = arx_in(i,j,2)  ! Pack up local array values
          enddo
        enddo
!$OMP END PARALLEL DO
!
! Send to process "idestb", with same myrankx,y and myrankz-1:
          idestb= (myrankz-1)*nprocx*nprocy + myranky*nprocx + myrankx
!     
          call MPI_Send(shado_B(1,1),nx_loc*ny_loc,MPI_REAL,
     &                  idestb,itagb,MPI_COMM_WORLD,ierror)
!
! Receive from process "idestb" as well:
          call MPI_Recv(shado_T(1,1),nx_loc*ny_loc,MPI_REAL,
     &                  idestb,itagt,MPI_COMM_WORLD,istatus,ierror)
!
!$OMP PARALLEL DO PRIVATE(i,j)
          do j=2,ny_loc+1
            do i=2,nx_loc+1
              arx_in(i,j,1) = shado_T(i-1,j-1)
            enddo
          enddo
!$OMP END PARALLEL DO
!
      endif
      lbottom = .true.
      if(.not.ltop) go to 101
!
!
!
  103 return
      end
