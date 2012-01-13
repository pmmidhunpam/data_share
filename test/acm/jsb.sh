cat jsb.data  | sed 's/[<>]/ & /'  | awk '{if($1 > $3){if($2 = ">")$2 = "<";else $2 = ">";print $3" "$2" "$1}else{print $1" "$2" "$3}}'  | sort | uniq  | sed 's/[<>]/<>/' | uniq -d > jsb.data.tmp

num=`cat jsb.data.tmp | wc -l`

cat jsb.data.tmp | sed 's/ <> /\n/' | uniq -c | awk '{if($1 == '$num'){print "yes, we got caipan: "$2; has_caipan=1}}END{if(has_caipan != 1)print "no caipan"}'

