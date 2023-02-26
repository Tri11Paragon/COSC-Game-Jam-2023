#!/bin/bash
#mkdir cmake-build-emrelease
rm -fr cmake-build-emrelease
emcmake cmake -DCMAKE_BUILD_TYPE=Release -DUSE_EXTRAS=ON -S ./ -B ./cmake-build-emrelease
cd cmake-build-emrelease
emmake make -j 16

#cp FinalProject.js /var/www/html/emscript/
#cp FinalProject.data /var/www/html/emscript/
#cp FinalProject.wasm /var/www/html/emscript/
rsync COSC_Game_Jam_2023.js administrator@70.51.110.192:/var/www/tld/games/2023-cosc-gamejam -e 'ssh -p 256 -i ~/Documents/Keys/supercomputer' --progress
rsync COSC_Game_Jam_2023.data administrator@70.51.110.192:/var/www/tld/games/2023-cosc-gamejam -e 'ssh -p 256 -i ~/Documents/Keys/supercomputer' --progress
rsync COSC_Game_Jam_2023.wasm administrator@70.51.110.192:/var/www/tld/games/2023-cosc-gamejam -e 'ssh -p 256 -i ~/Documents/Keys/supercomputer' --progress


