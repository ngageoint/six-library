Under linux, I have had to explicitly link with -lpthread.  
If you get ThreadPosix linker errors, try sticking -lpthread at the
end of your list of libraries

