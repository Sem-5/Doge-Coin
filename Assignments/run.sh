

adv=('1' '2')
gamma=('0.25' '0.5' '0.75' '1')
tadv=('90000' '40000' '23333.33333' '15000' '10000' '6666.66666' '4285.714285' '2500' '1111.11111')

for a in "${adv[@]}"
do
	for g in "${gamma[@]}"
	do
		for t in "${tadv[@]}"
		do
			echo $a $g $t
			cd ./build/
			./sim 50 0.75 5e3 1e4 5e5 $a $g $t 
			cd ../mpucalc
			echo $a $g $t >> op.txt
			python3 calc.py >> op.txt
			cd ..
		done
	done
done