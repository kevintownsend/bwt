check : human_g1k_v37.fasta
	head human_g1k_v37.fasta
	../example
	

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
