#!/bin/sh
#
#	define the standout() and standend() escape sequences...
#
SO="[7m"
SE="[0m"
stty pass8

#biff n

while true
do
clear
echo "---------------------------------------------"
echo "          ${SO} 交大資工系 BBS 服務項目 ${SE}"
echo "---------------------------------------------"
echo "
   ${SO}[0]${SE} 本站公佈欄

   ${SO} 圖書查詢系統 ${SE}
   [1] 交通大學		[5] 國立中央圖書館 (高手用之, 因為很複雜)
   [2] 清華大學  	[6] 台北市立圖書館
   [3] 台灣大學         [7] 中央研究院              
   [4] 成功大學              

   ${SO}[97]${SE} 設定 1Discuss 所用之編輯器
   ${SO}[98]${SE} login 人數統計圖表歷史檔
   ${SO}[99]${SE} 今日 login 人數統計圖表
    ${SO}[S]${SE} 將 1Discuss 內之討論區排序

   ${SO}[Q]${SE} Exit.    
"
#   ${SO}[99]${SE} 進站次數排行榜 

echo -n "請選擇 [Q]: " 

        if read CHOICE
           then
	    clear
            time_stamp=`sdate`
            case "${CHOICE}"
              in
                '')
                  break
                  ;;
                'Q')
                  break
                  ;;
		'q')
		  break
		  ;;
                0)
		  LC_CTYPE=iso_8859_1
		  export LC_CTYPE
		  echo "$time_stamp faqview $USER" >> bbslog
		  cd 0Announce
		  faqview '    交通大學資訊工程學系 BBS 公佈欄 (perl 版)'
		  cd ..
                  ;;
                1)
                  echo "$time_stamp bbsnet $USER 交大圖書館" >> bbslog
                  echo "${SO} 連往交大圖書館 ${SE}"
                  rlogin lib1.nctu.edu.tw -l library
                  #expect /expect/nctu_lib
                  ;;
                2)
                  echo "$time_stamp bbsnet $USER 清大圖書館" >> bbslog
                  echo "${SO} 連往清大圖書館 ${SE}"
                  echo "請用 ${SO}search${SE} login"
                  telnet 140.114.72.2
                  #expect /expect/nthu_lib
                  ;;
                3)
                  echo "$time_stamp bbsnet $USER 台大圖書館" >> bbslog
                  echo "${SO} 連往台大圖書館 ${SE}"
                  echo "請用 ${SO}library${SE} login"
                  #rlogin asleep.ntu.edu.tw -l reader
                  telnet 140.112.196.20 
                  ;;
                4)
                  echo "$time_stamp bbsnet $USER 成大圖書館" >> bbslog
                  echo "${SO} 連往成大圖書館 ${SE}"
                  echo "請用 ${SO}OPAC${SE} login"
                  telnet 140.116.207.1
                  #expect /expect/ncku_lib
                  ;;
                5)
                  echo "$time_stamp bbsnet $USER 中央圖書館" >> bbslog
                  echo "${SO} 連往國立中央圖書館 ${SE}"
                  echo "\
如果要退出中央圖書館的系統, 請在「檢索鍵」提示符號下鍵入 ${SO}.X${SE} 
然後在 「command」 提示符號下鍵入 ${SO} logoff ${SE}"
                  echo -n "請按 ENTER 繼續"
                  read junk
                  #telnet 192.83.186.1
                  expect -f /expect/clib
                  ;;
                6)
                  echo "$time_stamp bbsnet $USER 台北市立圖書館" >> bbslog
                  echo "${SO} 連往台北市立圖書館 ${SE}"
                  echo "請用 ${SO}opacnet${SE} login"
                  #telnet 192.83.187.1
                  expect -f /expect/tmlib
                  ;;
                7)
                  echo "$time_stamp bbsnet $USER 中研院圖書館" >> bbslog
                  echo "${SO} 連往中研院圖書館 ${SE}"
                  rlogin las.sinica.edu.tw -l chinese
                  ;;
		97) echo "          >>>   編輯器設定畫面   <<<     " 
                    echo "============================================="
                    echo "     (1) ${SO}ve${SE}   ---- BBS 內所用的編輯器"
		    echo "     (2) ${SO}cvi${SE}  ---- vi"
                    echo "     (3) ${SO}cjoe${SE} ---- joe"
                    echo "============================================="
                    echo -n "請選擇 1 - 3 的數目 (請耐心等候) ==> " 
                    if read EDIT 
	            then
	                qEDIT=`cat /home/$USER/editor`
                        case ${EDIT} in
                        1) qEDIT="/bin/ve" ;;
                        2) qEDIT="/bin/cvi" ;;
                        3) qEDIT="/bin/cjoe" ;;
                        *) echo  "只能選擇 1 - 3 的數目"  ;;
                        esac

                        echo  "目前使用之編輯器為 ${SO} $qEDIT ${SE}"
                    fi

		    echo $qEDIT > /home/$USER/editor
                    echo -n "請按 <enter> 繼續" 
                    read junk
                    echo "$time_stamp bbsnet $USER set_editor" >> bbslog
		    ;;
                
                98) echo -n "月份 (1..12) ==>"
		    read mon
		    echo -n "日期 (1..30) ==>"
		    read day
		    rmore stat/login.$mon.$day 
                    echo "$time_stamp bbsnet $USER 統計表" >> bbslog
                    echo -n "請按 ${SO}ENTER${SE} 繼續 "
                    read junk  
                  ;;

                99) rmore stat/login 
                    echo "$time_stamp bbsnet $USER 統計表" >> bbslog
                    echo -n "請按 ${SO}ENTER${SE} 繼續查詢昨日統計表 "
                    read junk  
		    clear
		    rmore stat/login.yesterday
                    echo -n "請按 ${SO}ENTER${SE} 繼續 "
                    read junk  
                  ;;
		s|S) 
		    if [ -f home/$USER/.newsrc ] ; then
			cp home/$USER/.newsrc tmp/$USER
			sort tmp/$USER > home/$USER/.newsrc
			rm tmp/$USER
		    fi
		    ;;
                *)
          	  echo ""
		  echo "${SO} 看不懂您選的 ${CHOICE}, 再試一次吧 .${SE}"
		  read junk
		  ;;
            esac
          else
            exit
        fi
done

clear

