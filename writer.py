#!/usr/bin/python3
# The tags are in uppercase due to how they were used the assignment description,
# even though, in html coding, they would generally be lowercase
import sys, os

# PURPOSE:: makes a string tag out of whatever character you give it
# ARGUMENTS:: the character to make a tag out of
# RETURNS:: the tag
def tag(char):
    return '<' + char + '>'

# PURPOSE:: converts the source_list into 1 string
# and replaces original list with the required tagged strings from changes list
#
# ARGUMENTS:: the list to combine into a string, and the list containing the
# the same string with tags added
# RETURNS:; the string containing all list elements, and
# the one with the tags added to it
def string_w_tags(source_list, changes_list):
    
    string = ''.join(source_list)
    
    for line in changes_list:
        if(line.find('<') == -1):
            continue
    
        string = string.replace( line[3: len(line) - 4], line)
    return string

# PURPOSE:: converts string to list with each substring
# as long as each source_list string + tags, but still containing the
# one_line's characters in it
#
# ARGUMENTS:: the long string, and the source_list to base new list on
# RETURNS:: the new list converted from the string
# NOTE:: first list must have the same content as the string - any tags
def string_to_list (one_line, source_list):
    
    updated = []
    i = 0
    for line in source_list:
        total_chars = len(line) + i
        j = i
      
        while i < total_chars:
            if(one_line[i: i + 3] in ("<B>", "<I>", "<U>") ):
                total_chars = total_chars + 3
        
            elif(one_line[i : i + 4] in ("</B>", "</I>", "</U>") ):
                total_chars = total_chars + 4
            
            i = i + 1
        updated.append(one_line[j : i])
    return updated

# Main program
################################################################################
lines = ["<HTML>", "<HEAD>", "<TITLE>", sys.argv[1], 
    "</TITLE>", "</HEAD>", "<BODY>"]

with open("myfifo", "r") as myfifo:
    for line in myfifo:
        lines.append(line.strip('\n'))

lines.extend(["</BODY>", "</HTML>"])

special = []

# takes each string from info, and adds a beginning and end tag to it,
# so that it can be used to replace text

if(os.path.isfile(sys.argv[1] + ".info") == 1): 
   with open(sys.argv[1] + ".info", "r") as info:
        for line in info:
            if(line[0] in ('B', 'U', 'I') ):
                special.append( tag(line[0]) + 
                    line[2:].strip('\n') + 
                    tag('/' + line[0])) 

updated = string_to_list( string_w_tags(lines, special), lines)

# the code below is mainly to correct an issue with output
#
# if there is a word at the end that needs to be tagged, without
# this, it will add the </> tag (the last part of a tag)
# to the start of the next string instead of 
# at the end of the current string
#
for line in reversed(updated):
    
    end_tag = '';
    if(len(line) >= 4):
        if(line[0:4] in ("<B>", "<U>", "<I>") ):
            end_tag = line[0:4]
            line = line.replace(line[0:4], '', 1)
    
    elif(len(line) >= 5):
        if(line[1:5] in ("</B>", "</U>", "</I>") ):
            end_tag = line[1:5]
            line = line.replace(line[1:5], '', 1)
        
    line = line + end_tag

with open("return_fifo", "w") as fifo:
    for line in updated:
        fifo.write(line + "\n")
