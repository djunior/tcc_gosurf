function abs(v) {
	return v < 0 ? -v : v
} 

function calculateDiffPercent(diff,base) {
	return 100*(1 - abs(diff-base)/base)
}

function standard_deviation(values,mean,count) {
	sd_sum = 0
	for (key in values) {
		sd_sum += ((values[key] - mean)^2) / (count)
	}
	return sqrt(sd_sum)
}

function variance(values,count) {
	variance_sum = 0
	for (key1 in values) {
		for(key2 in values) {
			variance_sum += ((values[key1] - values[key2])^2)/2
		}
	}
	return variance_sum/(count^2)
}

BEGIN{
	line_height_count = 0
	manual_line_height_count = 0
	manual_height_average = 0;
	manual_height_sd = 0;

	manual_pixel_average = 0;
	manual_line_pixel_count = 0;

	alg_line_height_count = 0;
	alg_height_average = 0;
	alg_height_sd = 0;

	alg_pixel_average = 0;
	alg_line_pixel_count = 0;
}

FNR==NR && /Waves/{manual_count=$3; next} 
/Waves/{alg_count=$3} 

FNR==NR && /meters/{
	manual_height_average+=$3
	manual_height_values[++manual_line_height_count] = $3
	next
}
/meters/{
	alg_height_average += $3
	alg_height_values[++alg_line_height_count] = $3
}

FNR==NR && /pixels/{
	manual_pixel_average+=$3
	manual_pixel_values[++manual_line_pixel_count] = $3
	next
}
/pixels/{
	alg_pixel_average += $3
	alg_pixel_values[++alg_line_pixel_count] = $3
}

END{
	print "Numero de ondas encontradas (manual): " manual_count
	print "Numero de ondas encontradas (algoritmo): " alg_count

	print "Porcentagem de ondas encontradas (algoritmo/manual): " calculateDiffPercent(alg_count,manual_count) "%"

	line_height_count = manual_line_height_count

	manual_height_average /= manual_line_height_count;
	alg_height_average /= alg_line_height_count;

	print "Altura media em metros (manual): " manual_height_average
	print "Altura media em metros (algoritmo): " alg_height_average

	manual_pixel_average /= manual_line_pixel_count;
	alg_pixel_average /= alg_line_pixel_count;

	print "Altura media em pixels (manual): " manual_pixel_average
	print "Altura media em pixels (algoritmo): " alg_pixel_average

	manual_height_sd = standard_deviation(manual_height_values,manual_height_average,manual_line_height_count)
	manual_pixel_sd = standard_deviation(manual_pixel_values,manual_pixel_average,manual_line_pixel_count)
	
	#manual_height_variance = manual_height_sd^2
	#manual_height_variance = variance(manual_height_values,line_height_count)

	alg_height_sd = standard_deviation(alg_height_values,alg_height_average,alg_line_height_count)
	alg_pixel_sd = standard_deviation(alg_pixel_values,alg_pixel_average,alg_line_pixel_count)
	
	#alg_height_variance = alg_height_sd^2
	#alg_height_variance = variance(alg_height_values,line_height_count)

	print "Desvio padrao (altura manual em metros): " manual_height_sd
	print "Desvio padrao (altura algoritmo em metros): " alg_height_sd

	print "Desvio padrao (altura manual em pixels): " manual_pixel_sd
	print "Desvio padrao (altura algoritmo em pixels): " alg_pixel_sd
}
