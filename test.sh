cat ./test/test.pas | ./spl 2>spl.ll

rm /var/folders/b0/3bwqz15d0bd5t43y1hwzg2pm0000gn/T/*.dot
rm ./*.dot
llvm-as < spl.ll | opt -analyze -view-cfg
mv /var/folders/b0/3bwqz15d0bd5t43y1hwzg2pm0000gn/T/*.dot .
dot *.dot -T png -o cfg.png

lli spl.ll
