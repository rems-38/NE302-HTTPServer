#/bin/bash

for file in `ls testFile/*.txt`
do
    if [ `./sg $file` = "-1" ]
    then
        echo "$file failed"
    fi
done
