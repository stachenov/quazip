# Github pages release process

```
sudo apt-get install doxygen graphviz
cp -r quazip quazip-docs
cd quazip-docs
git checkout gh-pages
cd ../quazip
( cat Doxyfile ; printf "OUTPUT_DIRECTORY=../quazip-docs\nHTML_OUTPUT=.\n" ) | doxygen -
cd ../quazip-docs
git add -A
git commit -m "Update docs"
git push
```