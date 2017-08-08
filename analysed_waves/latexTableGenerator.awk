BEGIN{
	FS=": "
}

/Numero.*manual/{
	manual_number_waves = $2
}

/Numero.*algoritmo/{
	alg_number_waves = $2
}

/Altura.*metros.*manual/{
	manual_height_meters = $2
}

/Altura.*metros.*algoritmo/{
	alg_height_meters = $2
}

/Altura.*pixels.*manual/{
	manual_height_pixels = $2
}

/Altura.*pixels.*algoritmo/{
	alg_height_pixels = $2
}

/Desvio.*manual.*metros/{
	manual_sd_meters = $2
}

/Desvio.*algoritmo.*metros/{
	alg_sd_meters = $2
}

/Desvio.*manual.*pixels/{
	manual_sd_pixels = $2
}

/Desvio.*algoritmo.*pixels/{
	alg_sd_pixels = $2
}

END{
	print "\\begin{tabular}{|l|l|l|}"
	print "\\hline"
	print "Parâmetros & Valores (manuais) & Valores (algoritmo) \\\\ \\hline"
	print "Número de Ondas Encontradas & " manual_number_waves " & " alg_number_waves " \\\\ \\hline"
	print "Altura média das Ondas (pixels) & " manual_height_pixels " & " alg_height_pixels " \\\\ \\hline"
	print "Desvio Padrão (pixels) & " manual_sd_pixels " & " alg_sd_pixels " \\\\ \\hline"
	print "Altura média das Ondas (metros) & " manual_height_meters " & " alg_height_meters " \\\\ \\hline"
	print "Desvio Padrão (metros$^{2}$) & " manual_sd_meters " & " alg_sd_meters " \\\\ \\hline"
	print "\\end{tabular}"
}