

*Bad* test files were created with the following script (run from the root of the repository).

```shell
for x in $(fd "\.nitf$" | grep -vi sidd);
do
    perl -nE 'if (/(<SI[CD]D.*?>.*<\/SI[CD]D>)/) {$b=$1; $b=~m/.*?:(\d+\.\d+.\d+)/g; $c=$1; $c=~s/\.//g; say $c; $b=~s{(<SICD.*?>)}{$1 . "<fake_tag />"}ex;  $fname="six/modules/c++/six.sicd/tests/sample_xml/sicd${c}-bad.xml"; open my $fh, ">", $fname or die $!; print $fh $b; system("xmllint --format ${fname} -o ${fname}")};' $x ;
done
```
