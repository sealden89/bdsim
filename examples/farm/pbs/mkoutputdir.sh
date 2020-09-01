mk_output_dir(){
    THEDIR=$1
    DESIREDDIR=$1
    X=1
    while [ -d $THEDIR ]
      do
      printf "%s - already exists\r\n" "$THEDIR"
      THEDIR=$DESIREDDIR'_'$X
      ((X++))
    done
    printf "Creating directory %s\n" "$THEDIR"

    #make the directory
    mkdir -p $THEDIR

    #make group read writable
    chmod g+rw $THEDIR

    #can only be one 'echo' to return the output
    #echo $THEDIR
    OUTPUTDIR=$THEDIR
}
