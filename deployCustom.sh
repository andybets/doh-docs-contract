if [ $# -le 1 ];
  then
    echo ERROR: parameters missing, pass api as first parameter and account as second parameter
    exit 1;
  else
    cleos -u "$1" set code "$2" ../build/dohdocs/dohdocs.wasm
    cleos -u "$1" set abi "$2" ../build/dohdocs/dohdocs.abi
fi
