#!/bin/bash
inotifywait -qq -e modify $0 *.c *.js *.tem
echo '-------------------------------------------------'
#node --harmony --harmony_destructuring --strong_mode test.js
#node --harmony --harmony_destructuring --debug --expose_debug_as=v8debug include.js
#node --harmony --harmony_destructuring --expose_debug_as=v8debug test.js
#rm tags > /dev/null
#ctags --file-scope=no c.c
#cc --std=c99 -O3 -march=native ijs.c -o ijsc
#cc --std=c99 -O2 skim.c -o skim
rm skim
cc --std=c99 -O2 skim.c -o skim
cc --std=c99 -O2 -shared -fPIC skim.c -o libskim.so
#time ijsc m2.js
#skim m1.js && cat out.js
#skim m4.js && diff m4.js out.js
#skim jq.js && diff jq.js out.js
#skim jq.js -t
#skim jquery-1.11.3.js -t
#skim t1.tem t1.out
rm m1.out
skim m1.js m1.out
cat m1.out
echo '- - - - - - - - - - - - - - - - - - - - - - - - - - - - '
skim -e m1.js m1.oute
cat m1.oute

# run tests
pushd . > /dev/null
cd test > /dev/null
for f in *; do
  if [[ $f != *.out ]]; then
    if [[ $f != *.oute ]]; then
      if [[ $f != *.run ]]; then
        #echo '------------'
        #ls -l $f
        ../skim $f $f.out.run > /dev/null
        ../skim -e $f $f.oute.run > /dev/null
        diff $f.out $f.out.run | colordiff
        diff $f.oute $f.oute.run | colordiff
      fi
    fi
  fi
done
popd > /dev/null
#skim -e test/.js m1.oute

#node skim.js
#node \
  #--harmony \
  #--harmony_destructuring_assignment \
  #ijs.js
  #--trace-hydrogen \
  #--trace-phase=Z \
  #--trace-deopt \
  #--code-comments \
  #--hydrogen-track-positions \
  #--redirect-code-traces \
  #--redirect-code-traces-to=code.asm \
  #--print-opt-code \
  #--allow-natives-syntax \
  #--print-code \
  #--code-comments \
  #--print-opt-code \
#node --harmony_destructuring --strong_mode m2.js
#lessc css/style.less css/style.css
#midori -e Reload
#cat style.css
#xdotool search --name chromium windowactivate --sync
#xdotool search --name --onlyvisible chromium key --clearmodifiers CTRL+R
. $0
