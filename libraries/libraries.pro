TEMPLATE = subdirs

exists(../trademarks) {
	SUBDIRS = kiss \
		  extraGUI \
                  ../trademarks/simulator \
                  program_visualizer
}
!exists(../trademarks) {
SUBDIRS = kiss \
          kiss_sim \
          extraGUI \
          program_visualizer
}
