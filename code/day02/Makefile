server:
# 反斜线表示换行，便于阅读
	g++ util.cpp client.cpp -o client && \
		g++ util.cpp server.cpp -o server
# clean 清除上次的 make 命令产生的目标文件。为了避免称为 make 的默认目标，不成文的规矩是 clean 放在文件最后。
clean:
	rm server && rm client
