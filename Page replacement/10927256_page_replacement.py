# 全域變數
pFrameList = [ [] for i in range(6) ]
pFaultList = [ [] for i in range(6) ]
output = [ [] for i in range(6) ] # 放每個方法的[pagefault,pageReplace]
# print(pReplaceList)

def selectWay( way, frameSize, pageQ )  :
	# 複製pageQ的內容出來，避免做POP的時候改到原始的內容
	# deep copy
	temp = []
	for i in range( len(pageQ) ) :
		temp.append(pageQ[i])

	if way == 1 :
		FIFO( frameSize, temp )
	elif way == 2 :
		LRU( frameSize, temp )
	elif way == 3 :
		LFUandFIFO( frameSize, temp )
	elif way == 4 :
		MFUandFIFO( frameSize, temp )
	elif way == 5 :
		LFUandLRU( frameSize, temp )

def checkPageFault( page, pageFrame, pageFault ) :
	find = False 
	index = -1
	for i in range( len(pageFrame) ) :
		if pageFrame[i] == page :
			index = i
			find = True

	if not find : 
		pageFault += 1

	return find, pageFault, index

def intoStr( pageFrame ) :
	# 複製pageFrame的內容出來，避免做reverse的時候改到原始的內容
	# deep copy
	pf = []
	for i in range(len(pageFrame)) :
		pf.append(pageFrame[i])
	pf.reverse()

	temp = ''
	for i in range( len(pf) ) :
		temp = temp + str(pf[i])

	return temp

def FIFO( frameSize, pageQ ) :
	
	pFault = []
	pFrame = []

	pageFrame = [] 
	pageFault = 0
	pageReplace = 0 
	index = -1
	
	while len(pageQ) > 0 :
		cur = pageQ[0]
		pageQ.pop(0)
		
		find, pageFault, index = checkPageFault( cur, pageFrame, pageFault )
		
		if not find : # not find in pageFrame
			pFault.append('F') # 表示發生pageFault
			if len(pageFrame) < frameSize :
				pageFrame.append(cur)
			elif len(pageFrame) == frameSize :
				pageFrame.pop(0) # 如果pageFrame滿了，就將最舊的page pop掉
				pageFrame.append(cur)
				pageReplace += 1 # 表示發生pageReplace
		else :
			pFault.append('T')

		pFrame.append( intoStr( pageFrame ) )


	global pFrameList 
	pFrameList[0] = pFrame

	global pFaultList 
	pFaultList[0] = pFault

	global output 
	output[0] = [pageFault,pageReplace]
	
def LRU( frameSize, pageQ ) :
	pFault = []
	pFrame = []

	pageFrame = [] 
	pageFault = 0
	pageReplace = 0 
	index = -1
	while len(pageQ) > 0 :
		cur = pageQ[0]
		pageQ.pop(0)
		
		find, pageFault, index = checkPageFault( cur, pageFrame, pageFault )
		if not find : # not find in pageFrame
			pFault.append('F') # 表示發生pageFault
			if len(pageFrame) < frameSize :
				pageFrame.append(cur)
			elif len(pageFrame) == frameSize :
				pageFrame.pop(0)    # 如果pageFrame滿了，就將最舊的page pop掉
				pageFrame.append(cur)
				pageReplace += 1    # 表示發生pageReplace
		else : 
			pFault.append('T') 
			temp = pageFrame[index]
			pageFrame.pop(index)
			pageFrame.append(temp) # 更新time stamp，移到pageFrame的最後一個

		pFrame.append( intoStr( pageFrame ) )


	global pFrameList 
	pFrameList[1] = pFrame

	global pFaultList 
	pFaultList[1] = pFault

	global output 
	output[1] = [pageFault,pageReplace]	

def findmin( frameCounter ) :
	index = -1
	min = 100000000
	for i in range( len(frameCounter) ) :
		if frameCounter[i] < min :
			min = frameCounter[i]
			index = i

	return index

def LFUandFIFO( frameSize, pageQ ) :
	pFault = []
	pFrame = []

	frameCounter = []
	pageFrame = [] 
	pageFault = 0
	pageReplace = 0 
	index = -1
	while len(pageQ) > 0 :
		cur = pageQ[0]
		pageQ.pop(0)
		
		find, pageFault, index = checkPageFault( cur, pageFrame, pageFault )
		if not find : # not find in pageFrame
			pFault.append('F') # 表示發生pageFault
			if len(pageFrame) < frameSize :
				pageFrame.append(cur)
				frameCounter.append(0)
			elif len(pageFrame) == frameSize :
				index = findmin( frameCounter )  # 如果pageFrame滿了，就將counter最小的page pop掉
				pageFrame.pop(index)   # 將counter最小的page從pageFrame中移除
				frameCounter.pop(index)
				pageFrame.append(cur)  # 新增目前的page至pageFrame中的最尾端
				frameCounter.append(0)
				pageReplace += 1  # 表示發生pageReplace
		else : 
			pFault.append('T')
			frameCounter[index] += 1 # counter+1，表示此page有被reference到

		pFrame.append( intoStr( pageFrame ) )


	global pFrameList 
	pFrameList[2] = pFrame

	global pFaultList 
	pFaultList[2] = pFault

	global output 
	output[2] = [pageFault,pageReplace]
	
def findMax( frameCounter ) :
	index = -1
	max = -1
	for i in range( len(frameCounter) ) :
		if frameCounter[i] > max :
			max = frameCounter[i]
			index = i

	return index

def MFUandFIFO( frameSize, pageQ ) :
	pFault = []
	pFrame = []

	frameCounter = []
	pageFrame = [] 
	pageFault = 0
	pageReplace = 0 
	index = -1
	while len(pageQ) > 0 :
		cur = pageQ[0]
		pageQ.pop(0)
		
		find, pageFault, index = checkPageFault( cur, pageFrame, pageFault )
		if not find : # not find in pageFrame
			pFault.append('F')  # 表示發生pageFault
			if len(pageFrame) < frameSize :
				pageFrame.append(cur)
				frameCounter.append(0)
			elif len(pageFrame) == frameSize :
				index = findMax( frameCounter ) # 如果pageFrame滿了，就將counter最大的page pop掉
				pageFrame.pop(index)            # 將counter最大的page從pageFrame中移除
				frameCounter.pop(index)
				pageFrame.append(cur)           # 新增目前的page至pageFrame中的最尾端
				frameCounter.append(0)
				pageReplace += 1                # 表示發生pageReplace
		else : 
			pFault.append('T')
			frameCounter[index] += 1  # counter+1，表示此page有被reference到
			
		pFrame.append( intoStr( pageFrame ) )


	global pFrameList 
	pFrameList[3] = pFrame

	global pFaultList 
	pFaultList[3] = pFault

	global output 
	output[3] = [pageFault,pageReplace]

def LFUandLRU( frameSize, pageQ ) :
	pFault = []
	pFrame = []

	frameCounter = []
	pageFrame = [] 
	pageFault = 0
	pageReplace = 0 
	index = -1
	while len(pageQ) > 0 :
		cur = pageQ[0]
		pageQ.pop(0)
		
		find, pageFault, index = checkPageFault( cur, pageFrame, pageFault )
		if not find : # not find in pageFrame
			pFault.append('F') # 表示發生pageFault

			if len(pageFrame) < frameSize :
				pageFrame.append(cur)
				frameCounter.append(0)
			elif len(pageFrame) == frameSize :  
				index = findmin( frameCounter ) # 如果pageFrame滿了，就將counter最小的page pop掉
				pageFrame.pop(index)  # 將counter最小的page從pageFrame中移除
				frameCounter.pop(index)
				pageFrame.append(cur) # 新增目前的page至pageFrame中的最尾端
				frameCounter.append(0)
				pageReplace += 1   # 表示發生pageReplace
		else : 
			pFault.append('T')
			frameCounter[index] += 1

			tempFrame = pageFrame[index]
			tempCounter = frameCounter[index]
			pageFrame.pop(index)
			pageFrame.append(tempFrame)
			frameCounter.pop(index)
			frameCounter.append(tempCounter)

		pFrame.append( intoStr( pageFrame ) )


	global pFrameList 
	pFrameList[4] = pFrame

	global pFaultList 
	pFaultList[4] = pFault

	global output 
	output[4] = [pageFault,pageReplace]

def writeWayTitle( way, file ) :
	if way == 1 :
		file.writelines( "--------------FIFO-----------------------" )
	elif way == 2 :
		file.writelines( "--------------LRU-----------------------" )
	elif way == 3 :
		file.writelines( "--------------Least Frequently Used Page Replacement-----------------------" )
	elif way == 4 :
		file.writelines( "--------------Most Frequently Used Page Replacement -----------------------" )
	elif way == 5 :
		file.writelines( "--------------Least Frequently Used LRU Page Replacement-----------------------" )

def outALLinfo( way, pageQ ) :

	outList = []
	temp = ''
	for i in range( len(pageQ) ) :

		if pFaultList[way-1][i] == 'F' :
			temp = str(pageQ[i]) + '\t' + str(pFrameList[way-1][i]) + '\t' + str(pFaultList[way-1][i])
		else:
			temp = str(pageQ[i]) + '\t' + str(pFrameList[way-1][i])

		outList.append(temp)
		temp = ''
	
	return outList

def writeContent( way, file, pageQ ) :
	writeWayTitle( way, file ) 
	file.writelines('\n')
	out = outALLinfo( way, pageQ )
	for i in range( len(pageQ) ) :
		file.writelines( out[i] )
		file.writelines('\n')

	# file.writelines('\n')
	file.writelines( "Page Fault = " + str(output[way-1][0]) )
	file.writelines( "  Page Replaces = " + str(output[way-1][1]) )
	file.writelines( "  Page Frames = " + str(frameSize) )
	file.writelines('\n')
	
def writeFile( name, way, frameSize, pageQ ):
	# 寫檔
	filename = "out_" + name + '.txt'
	file = open( filename, 'w+' )  # 建立一個新檔案，並寫入
	
	if way == 6 :
		for i in range(5) :
			writeContent( i+1, file, pageQ )
			if i != 4 :
				file.writelines('\n')
	else :
		writeContent( way, file, pageQ )
	
	file.close() # 關閉檔案

def readFile() :
	# 讀檔
	name = input("Please enter File Name (eg. input1 、 input1.txt) :\n")
	filename = name + '.txt'  # 輸入檔案名稱
	file = open( filename ,'r' )  # 開啟檔案
	
	data = []

	# 一次讀取一行文字，逐步讀取 
	for i in file:
		data.append(i.split())
		
	file.close() # 關閉檔案
		
	# print(data)
	way = int( data[0][0] )
	frameSize = int( data[0][1] )
	pageQ = data[1][0]	

	return name, way, frameSize, pageQ

if __name__ == "__main__" :
	
	name, way, frameSize, tempQ = readFile()
    
	pageQ = [] # 將q轉成list存
	for i in range( len(tempQ)) :
		pageQ.append(tempQ[i]) 

	if way == 6 : # all way 
		for i in range(5) :
			selectWay( i+1, frameSize, pageQ )
	else :
		selectWay( way, frameSize, pageQ ) 

	writeFile( name, way, frameSize, pageQ )
