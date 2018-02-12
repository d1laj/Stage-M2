for i in result/*.dot;
do
	dot -Tpdf $i -o ${i%.*}.pdf
done
