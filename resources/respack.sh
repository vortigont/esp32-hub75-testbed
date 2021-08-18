#!/bin/sh

USAGE="Usage: `basename $0` [-h] [-t embuitag] args"

# etag file
tags=etags.txt
# embui branch/tag name to fetch
embuitag="feat"
embuirepo='vortigont/EmbUI'

refresh_styles=0
refresh_js=0

# parse cmd options
while getopts ht: OPT; do
    case "$OPT" in
        h)
            echo $USAGE
            exit 0
            ;;
        t)
            echo "EmbUI tag is set to: $OPTARG"
            embuitag=$OPTARG
            ;;
        \?)
            # getopts issues an error message
            echo $USAGE >&2
            exit 1
            ;;
    esac
done


[ -f $tags ] || touch $tags

# check github file for a new hash
freshtag(){
    local url="$1"
    etag=$(curl -sL -I $url | grep etag | awk '{print $2}')
    echo "$url $etag" >> newtags.txt
    if [ $(grep -cs $etag $tags) -eq 0 ] ; then
        #echo "new tag found for $url"
        return 0
    fi
    #echo "old tag for $url"
    return 1
}


echo "Preparing resources for ESPEM FS image into ../data/ dir" 

mkdir -p ../data/css ../data/js

# собираем скрипты и стили из репозитория EmbUI
if freshtag https://github.com/${embuirepo}/raw/$embuitag/resources/data.zip ; then
    refresh_styles=1
    echo "EmbUI resources requires updating"
else
    echo "EmbUI is up to date"
fi

if freshtag https://github.com/${embuirepo}/raw/$embuitag/resources/html/js/lib.js ; then
    refresh_js=1
fi
if freshtag https://github.com/${embuirepo}/raw/$embuitag/resources/html/js/maker.js ; then
    refresh_js=1
fi
if freshtag https://github.com/${embuirepo}/raw/$embuitag/resources/html/js/dyncss.js ; then
    refresh_js=1
fi


# if any of the styles needs updating, than we need to repack both embui and local files
if [ $refresh_styles -eq 1 ] ; then

    echo "refreshing embui css files..."

    curl -sL https://github.com/${embuirepo}/raw/$embuitag/resources/data.zip > embui.zip
    # т.к. неизвестно что изменилось во фреймворке, скрипты или цсски, обновляем всё
    #unzip -o -d ../data/ embui.zip "css/*" "js/*"
    unzip -o -d ../data/ embui.zip "css/*" "js/tz*"

    # append our styles to the embui
#    for f in css/style_*.css
#    do
#        gzip -d ../data/css/$( basename $f).gz
#        cat $f >> ../data/css/$( basename $f)
#        gzip -9 ../data/css/$( basename $f)
#        touch -r $f ../data/css/$( basename $f).gz
#    done

    rm -f embui.zip
fi

if [ $refresh_js -eq 1 ] ; then
    echo "refreshing embui js files..."

    curl -sL https://github.com/${embuirepo}/raw/$embuitag/resources/html/js/lib.js > embui.js
    curl -sL https://github.com/${embuirepo}/raw/$embuitag/resources/html/js/maker.js >> embui.js
    curl -sL https://github.com/${embuirepo}/raw/$embuitag/resources/html/js/dyncss.js >> embui.js
    gzip -9 embui.js && mv -f embui.js.gz ../data/js/
fi


# update static files if newer
[ ! -f ../data/index.html.gz ]  || [ html/index.html -nt ../data/index.html.gz ] && gzip -9kf html/index.html && mv -f html/index.html.gz ../data/
#[ ! -f ../data/favicon.ico.gz ] || [ html/favicon.ico -nt ../data/favicon.ico.gz ] &&  gzip -9kf html/favicon.ico && mv -f html/favicon.ico.gz ../data/

mv -f newtags.txt $tags
