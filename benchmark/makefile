check : human_g1k_v37.raw
	time head -c 100 human_g1k_v37.raw | ../example
	time head -c 1000 human_g1k_v37.raw | ../example
	time head -c 10000 human_g1k_v37.raw | ../example
	time head -c 100000 human_g1k_v37.raw | ../example
	time head -c 1000000 human_g1k_v37.raw | ../example
	time head -c 10000000 human_g1k_v37.raw | ../example
	#memory issues on 32bit machine
	#time head -c 100000000 human_g1k_v37.raw | ../example
	

# Uncomment if you are in the Americas (the US, etc.)
FTP = ftp://ftp-trace.ncbi.nih.gov/1000genomes/ftp
# Uncomment if you are outside the Americas (Europe, etc.)
# FTP = ftp://ftp.1000genomes.ebi.ac.uk/vol1/ftp/

REFERENCE_GENOME = human_g1k_v37.fasta.gz

$(REFERENCE_GENOME) :
	wget $(FTP)/technical/reference/$(REFERENCE_GENOME)

%.fasta : %.fasta.gz
	gunzip -c $< > $@

human_g1k_v37.fasta : $(REFERENCE_GENOME)

human_g1k_v37.raw : human_g1k_v37.fasta removeReturn
	grep -e "^[^>]" < human_g1k_v37.fasta | removeReturn > human_g1k_v37.raw

removeReturn : removeReturn.c
	gcc -o removeReturn removeReturn.c

clean:
	rm -rf *.fasta
