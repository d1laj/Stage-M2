for i in colo2/result_7/*.dot;
do
	dot -Tpdf $i -o ${i%.*}.pdf
done
