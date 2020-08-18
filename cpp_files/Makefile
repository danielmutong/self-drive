OPENCVFLAG = `pkg-config --libs --cflags opencv4`
srcfile = main.cpp road.cpp single_line.cpp analysis.cpp selfdrive.cpp operation.cpp uhidkb.c
drive: ${srcfile}
	g++ ${srcfile} ${OPENCVFLAG} -lX11 -lxdo -lXtst -o drive

