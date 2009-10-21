# Our custom cmake preprocess steps - may replace gpp completely
# Required defines:
#	INFILE - file to be preprocessed
#	OUTFILE - file written after processing

# Load file into strings
file (READ ${INFILE} INDATA)

# Remove c-style comments
string (REGEX REPLACE "([/][*])([^/])+([/])" "" OUTDATA "${INDATA}")

# Write out result
file (WRITE ${OUTFILE} "${OUTDATA}")

