cmake -B buildvr -S . -DVCPKG_TARGET_TRIPLET=x64-windows-static-md -DBUILD_SKYRIMVR=On -DSKYRIMVR=On
cmake --build buildvr --config Release