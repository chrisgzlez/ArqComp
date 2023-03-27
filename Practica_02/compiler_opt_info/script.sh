files=$(ls | grep -E "*.txt")

for file in $files
do 
    echo "File: $file"
done
