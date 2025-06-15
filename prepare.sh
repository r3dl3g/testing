echo ======= New Prepare at $(date) ======= >> prepare.log
cmake -S. -Bbuild -DCMAKE_INSTALL_PREFIX=../libs -DTESTING_CONFIG_INSTALL=ON -DTESTING_BUILD_STATIC_MODULE_LIB=OFF $* 2>&1 | tee -a prepare.log
