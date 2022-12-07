if [ ! -d "build/" ];then
  sh make.sh
  else
    echo "run without make"
fi
./build/memalloc
