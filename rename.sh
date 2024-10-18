for file in $(find . -name '*supercrack*'); do
    newname=$(echo "$file" | sed 's/supercrack/supercrack/g')
    mv "$file" "$newname"
done

for dir in $(find . -type d -name '*supercrack*'); do
    newname=$(echo "$dir" | sed 's/supercrack/supercrack/g')
    mv "$dir" "$newname"
done
find . -type f -exec sed -i 's/supercrack/supercrack/g' {} +