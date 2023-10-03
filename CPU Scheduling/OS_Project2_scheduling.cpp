# include <iostream>
# include <fstream>
# include <queue>
# include <string>
# include <vector>

using namespace std ;

struct node {
    int PID ;
    int CPUtime ;
    int remain ;
    int Arrivaltime ;
    int priority;
    int wait ; // waiting time
    int tt ; // turnaround time 
} ;

int way ;
int timeslice ;

vector<node> pList ; // process list 
vector<vector<node>> fList ; // finish list
vector<vector<char>> gantte ; 

class Scheduler {
    
    public:
    void initial () {
        fList.resize(7) ; // finish list
        gantte.resize(7) ;
    } // initial

    void manager( int way, int timeslice ) {

        initial () ;
        sort( pList ) ;

        if ( way == 1 )  FCFS( fList[1], gantte[1] ) ;
        else if ( way == 2 )  RR( fList[2], gantte[2], timeslice ) ;
        else if ( way == 3 )  SJF( fList[3], gantte[3] ) ;
        else if ( way == 4 )  SRTF( fList[4], gantte[4] ) ;
        else if ( way == 5 )  HRRN( fList[5], gantte[5] ) ;
        else if ( way == 6 )  PPRR( fList[6], gantte[6], timeslice ) ;
        else if ( way == 7 ) {
            FCFS( fList[1], gantte[1] ) ;
            RR( fList[2], gantte[2], timeslice ) ;
            SJF( fList[3], gantte[3] ) ;
            SRTF( fList[4], gantte[4] ) ;
            HRRN( fList[5], gantte[5] ) ;
            PPRR( fList[6], gantte[6], timeslice ) ;
        } // else if 

    } // manager()

    private:

    void sort( vector<node> &list ) {
        // insertion sort
        // sort accroding to arrival time 
        // if arrival time same, sort accroding to pid
        for ( int i = 1 ; i < list.size() ; i++ ) {
            node key = list[i] ;
            bool success = false ; // whether insert is success
            int j = i - 1 ;
            while ( j >= 0 && ! success ) {

                if ( list[j].Arrivaltime > key.Arrivaltime ) {
                    list[j+1] = list[j] ; // right shift
                    j-- ;
                } // if 
                
                else if ( list[j].Arrivaltime == key.Arrivaltime ) {
                    
                    if ( list[j].PID < key.PID ) {
                        // swap
                        node temp = key ;
                        key = list[j] ;
                        list[j] = temp ;
                        
                    } // if 
                    else { // if (list[j].PID > key.PID)
                        list[j+1] = list[j] ; // right shift
                        j-- ;
                    } // if 
                    
                } // else if ()
                
                else success = true ;
                
            } // for 

            list[j+1] = key ;
        } // for 
        
    } // sort()

    void record( int pid, vector<char> &gantte ) {
        char p ; 
        if ( pid == -1 ) {
            p = '-' ;
        } // if 
        else if ( pid >= 0  && pid < 10 ) {
            p = pid + '0' ;
        } // 
        else if ( pid >= 10 && pid <= 36 ) {
            p = pid - 10 + 'A' ;
        } // 

        gantte.push_back(p) ;

    }// record()

    void FCFS( vector<node> &fList, vector<char> &gantte ) {

        int curTime = 0 ;   // 當前的時間點
        int intoTime = -1;  // Process進入cpu的時間點

        node CPU ;
        CPU.PID = -1 ; // 假設一開始的CPU是空的
        bool finish = false ; // whether all process finished

        for ( int i = 0 ; ! finish ; curTime++ ) {
            
            if ( CPU.PID != -1 ) { // 如果CPU不是空的
                if ( curTime == intoTime + CPU.CPUtime ) { // 檢查是否已經做完
                    CPU.tt = curTime - CPU.Arrivaltime ; 
                    fList.push_back(CPU) ;
                    CPU.PID = -1 ; // 重置CPU
                } // if 

            } // if  

            if ( CPU.PID == -1 ) { // 如果CPU是空的
                if ( i < pList.size() && pList[i].Arrivaltime <= curTime ) { // 檢查是否還有process可做
                    CPU.PID = pList[i].PID ;
                    CPU.CPUtime = pList[i].CPUtime ;
                    CPU.Arrivaltime = pList[i].Arrivaltime ;
                    intoTime = curTime ;
                    CPU.wait = intoTime - CPU.Arrivaltime ;
                    i++ ;
                } // 
                else if ( i >= pList.size() ) {
                    finish = true ; // 完成所有process
                    break; // 即刻跳出迴圈
                } // if 
            } // if ()

            record( CPU.PID, gantte ) ; // 紀錄當前正在執行的process            
        } // for 
    } //  FCFS()

    void RR( vector<node> &fList, vector<char> &gantte, int timeslice ) {
        
        int curTime = 0 ;   // 當前的時間點
        int intoTime = -1 ;  // Process進入cpu的時間點
        queue<node> readyQ ;
        node CPU ;
        CPU.PID = -1 ; // 假設一開始的CPU是空的
        bool finish = false ; // whether all process finished
        
        for ( int i = 0 ; ! finish ; curTime++ ) { 
            
            if ( CPU.PID != -1 ) CPU.remain-- ;

            while ( i < pList.size() && pList[i].Arrivaltime == curTime ) { // 檢查是否有新的process來
                node temp = pList[i] ;
                readyQ.push( temp ) ;
                i++ ;
            } // while
            
            if ( CPU.PID != -1 ) { // 如果CPU不是空的
                
                if ( curTime == intoTime + timeslice ) { // 檢查是否timeout
                    
                    if ( CPU.remain > 0 ) { // 發生timeout時，process未做完
                        while ( i < pList.size() && pList[i].Arrivaltime == curTime ) { // 檢查timeout時，是否有新的process來
                            node temp = pList[i] ;
                            readyQ.push( temp ) ;
                            i++ ;
                        } // while 

                        readyQ.push( CPU ) ;
                        CPU.PID = -1 ; // 重置cpu

                    } // if()
                
                    else { // 發生timeout時，procrss已經完成工作
                        CPU.tt = curTime - CPU.Arrivaltime ;
                        CPU.wait = CPU.tt - CPU.CPUtime ;
                        fList.push_back(CPU) ;
                        CPU.PID = -1 ; // 重置CPU
                    } // else 
                } // if 
                else if ( CPU.remain == 0 ) {
                    CPU.tt = curTime - CPU.Arrivaltime ;
                    CPU.wait = CPU.tt - CPU.CPUtime ;
                    fList.push_back(CPU) ;
                    CPU.PID = -1 ; // 重置CPU
                } // 
                    

            } // if  

            if ( CPU.PID == -1 ) { // 如果CPU是空的
                if ( ! readyQ.empty() ) { // 檢查是否還有process可做
                    CPU = readyQ.front() ; 
                    readyQ.pop() ;
                    intoTime = curTime ;
                } // 
                else if ( i >= pList.size() ){
                    finish = true ; // 完成所有process
                    break; // 即刻跳出迴圈
                } // if 
            } // if ()

            record( CPU.PID, gantte ) ; // 紀錄當前正在執行的process 

        } // for ()

    } // RR()

    void SJF( vector<node> &fList, vector<char> &gantte ) {

        int curTime = 0 ;   // 當前的時間點
        int intoTime = -1 ;  // Process進入cpu的時間點
        vector<node> readyQ ;
        node CPU ;
        CPU.PID = -1 ; // 假設一開始的CPU是空的
        bool finish = false ; // whether all process finished
        
        for ( int i = 0 ; ! finish ; curTime++ ) { 
            
            while ( i < pList.size() && pList[i].Arrivaltime == curTime ) { // 檢查是否有新的process來
                node temp = pList[i] ;
                readyQ.push_back( temp ) ;
                i++ ;
            } // while

            if ( CPU.PID != -1 ) { // 如果CPU不是空的
                if ( curTime == intoTime + CPU.CPUtime ) { // 檢查是否已經做完
                    CPU.tt = curTime - CPU.Arrivaltime ; 
                    fList.push_back(CPU) ;
                    CPU.PID = -1 ; // 重置CPU
                } // if 
                
            } // if  

            if ( CPU.PID == -1 ) { // 如果CPU是空的
                // system( "pause" ) ; 
                if ( readyQ.size() > 0 ) {
                    // INSERTION SORT, according to cputime
                    for ( int m = 1 ; m < readyQ.size() ; m++ ) {
                        node key = readyQ[m] ;
                        int j = m - 1 ;
                        while ( j >= 0 && readyQ[j].CPUtime > key.CPUtime  ) {
                            readyQ[j+1] = readyQ[j] ; // right shift
                            j-- ;
                        } // while     
                                
                        readyQ[j+1] = key ;

                    } // for

                    CPU = readyQ[0] ;
                    intoTime = curTime ;
                    CPU.wait = intoTime - CPU.Arrivaltime ;
                    // 從readyq中pop掉第一個
                    vector<node>::iterator it ;
                    it = readyQ.begin() ;
                    readyQ.erase( it ) ;

                } // if ()
                else if ( i >= pList.size() ) {
                    finish = true ; // 完成所有process
                    break; // 即刻跳出迴圈
                } // if 
            } // if ()

            record( CPU.PID, gantte ) ; // 紀錄當前正在執行的process 

        } // for ()

    } // SJF()

    void FindminRemain( vector<node> &readyQ, node &CPU, int &intoTime, int curTime )  {

        // 檢查當下正在做的process是不是剩餘cputime最小的
        node originCPU = CPU ;

        for ( int j = 0 ; j < readyQ.size() ; j++ ) {
            if ( readyQ[j].remain < CPU.remain ) {
                swap ( CPU, readyQ[j] ) ;
            } // if 
            else if ( readyQ[j].remain == CPU.remain ) {
                if ( readyQ[j].Arrivaltime < CPU.Arrivaltime ) {
                    swap ( CPU, readyQ[j] ) ;
                } // if 
                else if ( readyQ[j].Arrivaltime == CPU.Arrivaltime ){
                    if ( readyQ[j].PID < CPU.PID ) {
                        swap ( CPU, readyQ[j] ) ;
                    } // if ()
                } // else if ()
            } // else if  
            
        } // for()

        if ( originCPU.PID != CPU.PID ) {
            intoTime = curTime ;
        } // if ()

    } // FindminRemian()

    void SRTF( vector<node> &fList, vector<char> &gantte ) {
        int curTime = 0 ;   // 當前的時間點
        int intoTime = -1 ;  // Process進入cpu的時間點
        vector<node> readyQ ;
        node CPU ;
        CPU.PID = -1 ; // 假設一開始的CPU是空的
        bool finish = false ; // whether all process finished
        
        for ( int pindex = 0 ; ! finish ; curTime++ ) { 
            
            if ( CPU.PID != -1 ) CPU.remain-- ;

            while ( pindex < pList.size() && pList[pindex].Arrivaltime == curTime ) { // 檢查是否有新的process來
                node temp = pList[pindex] ;
                readyQ.push_back( temp ) ;
                pindex++ ;
            } // while

            if ( CPU.PID != -1 ) { // 如果CPU不是空的
                if ( CPU.remain == 0 ) { // 檢查是否已經做完
                    CPU.tt = curTime - CPU.Arrivaltime ; 
                    CPU.wait = CPU.tt - CPU.CPUtime ;
                    fList.push_back(CPU) ;
                    CPU.PID = -1 ; // 重置CPU
                } // if 
                else {
                    FindminRemain( readyQ, CPU, intoTime, curTime ) ;
                } // else
            } // if 

            
            if ( CPU.PID == -1 ) { 
                if ( readyQ.size() > 0 ) { // 檢查是否還有process可做

                    CPU = readyQ[0] ;  // 假設第一個是最小的
                    // 從readyq中pop掉第一個
                    vector<node>::iterator it ;
                    it = readyQ.begin() ;
                    readyQ.erase( it ) ;
                    intoTime = curTime ;
                    // 找出剩餘cputime最小的process
                    FindminRemain( readyQ, CPU, intoTime, curTime ) ;

                } // if 
                else if ( pindex >= pList.size() ){
                    finish = true ; // 完成所有process
                    break; // 即刻跳出迴圈
                } // if 
            } // if ()    
           
            record( CPU.PID, gantte ) ; // 紀錄當前正在執行的process 

        } // for() 
    } // SRTF()

    float ratio( node temp, int curTime ) {
        temp.wait = curTime - temp.Arrivaltime ;
        float a = temp.wait + temp.CPUtime ;
        float b = temp.CPUtime ;
        return a / b ;
    } // ratio 

    void FindMAXRatio( vector<node> &readyQ, node &CPU, int curTime ) {
        // 檢查當下正在做的process是不是ratio最大的

        CPU = readyQ[0] ; // 假設第一個是ratio最大的
        // 從readyq中pop掉第一個
        vector<node>::iterator it ;
        it = readyQ.begin() ;
        readyQ.erase( it ) ;

        for ( int j = 0 ; j < readyQ.size() ; j++ ) {
            float a = ratio( readyQ[j], curTime ) ;
            float b =  ratio( CPU, curTime ) ;
            if ( a > b ) {
                swap ( CPU, readyQ[j] ) ;
            } // if 
            else if ( a < b ) ;
            else { // if ( ratio( readyQ[j], curTime ) == ratio( CPU, curTime ) ) 
           
                if ( readyQ[j].Arrivaltime < CPU.Arrivaltime ) {
                    swap ( CPU, readyQ[j] ) ;
                } // if 
                else if ( readyQ[j].Arrivaltime == CPU.Arrivaltime ){
                    if ( readyQ[j].PID < CPU.PID ) {
                        swap ( CPU, readyQ[j] ) ;
                    } // if ()
                } // else if ()
            } // else if  
        } // 



    } // FindminRatio()

    void HRRN( vector<node> &fList, vector<char> &gantte ) {
        int curTime = 0 ;   // 當前的時間點
        int intoTime = -1 ;  // Process進入cpu的時間點
        vector<node> readyQ ;
        node CPU ;
        CPU.PID = -1 ; // 假設一開始的CPU是空的
        bool finish = false ; // whether all process finished
        
        for ( int pindex = 0 ; ! finish ; curTime++ ) { 
            
            while ( pindex < pList.size() && pList[pindex].Arrivaltime == curTime ) { // 檢查是否有新的process來
                readyQ.push_back( pList[pindex] ) ;
                pindex++ ;
            } // while

            if ( CPU.PID != -1 ) { // 如果CPU不是空的
                if ( curTime == intoTime + CPU.CPUtime ) { // 檢查是否已經做完
                    CPU.tt = curTime - CPU.Arrivaltime ;
                    CPU.wait = CPU.tt - CPU.CPUtime ;
                    fList.push_back(CPU) ;
                    CPU.PID = -1 ; // 重置CPU
                } // if 

            } // if  

            if ( CPU.PID == -1 ) { // 如果CPU是空的

                if ( readyQ.size() > 0 ) { // 檢查是否還有process可做

                    // 找出ratio最大的
                    FindMAXRatio( readyQ, CPU, curTime ) ;
                    intoTime = curTime ;

                } // if 
                else if ( pindex >= pList.size() ) {
                    finish = true ; // 完成所有process
                    break; // 即刻跳出迴圈
                } // if 

            } // if ()

            record( CPU.PID, gantte ) ; // 紀錄當前正在執行的process  

        } // for() 
    } // HRRN()

    bool IsOnlyPr( vector<node> readyQ ) {
        
        bool onlyOne = true ;
        int tempPR = readyQ[0].priority ;
        for ( int i = 0 ; i < readyQ.size() ; i++ ) {
            if ( tempPR != readyQ[i].priority ) 
                onlyOne = false ;
        } // for 

        return onlyOne ;

    } // IsOnlyPr
    
    void FindminPriority( vector<node> &readyQ, node &CPU, int &intoTime, int curTime, int &mode ) {


        // sort according to priority
        for ( int i = 1 ; i < readyQ.size() ; i++ ) {
            node key = readyQ[i] ;
            int j = i - 1 ;
            while ( j >= 0 && readyQ[j].priority > key.priority  ) {
                readyQ[j+1] = readyQ[j] ; // right shift
                j-- ;
            } // while     
                    
            readyQ[j+1] = key ;
        } // for

        // 如果本來就沒有工作
        if ( CPU.PID == -1 ) {

            // 如果readyQ中只有一個process是最小的Priority
            if ( IsOnlyPr( readyQ ) && readyQ.size() == 1 ) mode = 1 ;
            else mode = 2 ;

            CPU = readyQ[0] ;
            vector<node>::iterator it ;
            it = readyQ.begin() ;
            readyQ.erase( it ) ;
            intoTime = curTime ;

        } // if ()
        else if ( mode == 1 ) {
            if (  CPU.priority == readyQ[0].priority ) {
                // intoTime = curTime ;
                mode = 2 ;
            } // 
            else if ( CPU.priority > readyQ[0].priority ) { // pp被preemptive
                intoTime = curTime ;
                readyQ.push_back(CPU) ;
                CPU = readyQ[0] ;
                vector<node>::iterator it ;
                it = readyQ.begin() ;
                readyQ.erase( it ) ;
            } // if 
        } // else  
        else if ( mode == 2 ) { // RR被preemptive
            mode = 1 ; // 狀態切回pp
            intoTime = curTime ;
            readyQ.push_back(CPU) ;
            CPU = readyQ[0] ;
            vector<node>::iterator it ;
            it = readyQ.begin() ;
            readyQ.erase( it ) ;
        } // else 
    

    } // FindminPriority

    void PPRR( vector<node> &fList, vector<char> &gantte, int timeslice ) {
        int curTime = 0 ;   // 當前的時間點
        int intoTime = -1 ;  // Process進入cpu的時間點
        vector<node> readyQ ;
        node CPU ;
        CPU.PID = -1 ; // 假設一開始的CPU是空的
        bool finish = false ; // whether all process finished
        int mode = 1 ; // 1:pp , 2:rr
        
        for ( int pindex = 0 ; ! finish ; curTime++ ) { 

            if ( CPU.PID != -1 ) CPU.remain-- ;
            
            while ( pindex < pList.size() && pList[pindex].Arrivaltime == curTime ) { // 檢查是否有新的process來
                readyQ.push_back( pList[pindex] ) ;
                pindex++ ;
            } // while
            
            if ( CPU.PID != -1 ) { 
                 
                if ( CPU.remain == 0 ) { // 檢查是否已經做完
                    CPU.tt = curTime - CPU.Arrivaltime ; 
                    CPU.wait = CPU.tt - CPU.CPUtime ;
                    fList.push_back(CPU) ;
                    CPU.PID = -1 ; // 重置CPU
                    mode = 1 ; // 重置模式為pp
                } // if
    
                else if ( mode == 2 && curTime == intoTime + timeslice && CPU.remain > 0 ) { // 發生timeout時，process未做完
                    readyQ.push_back( CPU ) ;
                    CPU.PID = -1 ; // 重置cpu
                    mode = 1 ; // 重置模式為pp
                } // else

                else if ( mode == 2 && curTime < intoTime + timeslice &&  CPU.remain > 0 ) { // 發生timeout時，process未做完
                    // 檢查是否要被preemptive
                    bool Ispreemptive = false ;
                    for ( int i = 0 ; i < readyQ.size() ; i++ ) {
                        if ( readyQ[i].priority < CPU.priority ) {
                            Ispreemptive = true ;
                            break;
                        } // 
                    } // 

                    if ( Ispreemptive )  {
                        FindminPriority( readyQ, CPU, intoTime, curTime, mode ) ;
                    } // 
                } // else

                else if ( mode == 1 && readyQ.size() > 0 ) { // 檢查priority 
                    FindminPriority( readyQ, CPU, intoTime, curTime, mode ) ;
                } // else if 
                 
            } //if 

            if ( CPU.PID == -1 ) { // 如果CPU是空的

                if ( readyQ.size() > 0 ) { // 檢查是否還有process可做

                    // 找出priority最小的;
                    FindminPriority( readyQ, CPU, intoTime, curTime, mode ) ;
                    intoTime = curTime ;

                } // if 
                else if ( pindex >= pList.size() ) {
                    finish = true ; // 完成所有process
                    break; // 即刻跳出迴圈
                } // if 

            } // if ()
            
            record( CPU.PID, gantte ) ; // 紀錄當前正在執行的process  
            /*
            cout << "curtime : " << curTime << endl ;
            cout << "cpuid : " << CPU.PID << endl ;
            cout << "pr : " << CPU.priority << endl ;
            cout << "mode :" << mode << endl ;
            system( "pause" ) ; 
            */

        } // for()
    } // PPRR()


}; // Scheduler

void sortPID( vector<node> &list ) {
    // sort according to priority
    for ( int i = 1 ; i < list.size() ; i++ ) {
        node key = list[i] ;
        int j = i - 1 ;
        while ( j >= 0 && list[j].PID > key.PID  ) {
            list[j+1] = list[j] ; // right shift
            j-- ;
        } // while     
                
        list[j+1] = key ;
    } // for
} // sortPID

bool Readfile( string fname ) {
    fstream fin ;
    fname = fname + ".txt" ;

	fin.open( fname.c_str(), ios::in ) ; // open the file

	if ( ! fin.is_open() ) {  // if this file can't open
	    cout << "Failed to open file or this file doesn't exist." << endl ;
	    return false ;
	} // if

	fin >> way ;
	fin >> timeslice ;
    string title ;
	getline(fin, title) ; // the enter of row 1
    getline(fin, title) ; // ID CPU Burst  Arrival Time   Priority

	for ( node temp ; fin >> temp.PID >> temp.CPUtime >> temp.Arrivaltime >> temp.priority ; )  {
        temp.remain = temp.CPUtime ; 
        pList.push_back(temp) ;
    } // for 
	    
    // cout << pList.size() << endl ;
	fin.close() ; // close the file

    // check whether readfile is correct 
    /*
    for ( int i = 0 ; i < pList.size() ; i++ ) {
        cout << pList[i].PID << " " <<  pList[i].CPUtime << " " 
             << pList[i].Arrivaltime  << " " << pList[i].priority << endl ;
    } // for 
    
    system( "pause" ) ;
    */
    return true ;

} // Readfile()

string Wayname( int way ) {

    string name ;
    if ( way == 1 ) name = "FCFS" ;
    else if ( way == 2 ) name = "RR" ;
    else if ( way == 3 ) name = "SJF" ;
    else if ( way == 4 ) name = "SRTF" ;
    else if ( way == 5 ) name = "HRRN" ;
    else if ( way == 6 ) name = "PPRR" ;
    else if ( way == 7 ) name = "All" ;

    return name ; 

} // Wayname


void Outputfile( string fname, int way ) {

    fstream fout ;
    fname = "out_" + fname + ".txt" ;
    fout.open( fname.c_str(), ios::out ) ; // open the file

    if ( way == 7 ) { // 如果是all，就幫所有的方法都個別用pid排序
        for ( int i = 1 ; i < fList.size() ; i++ ) {
            sortPID( fList[i] ) ;
        } // for 
    } // if 
    else sortPID( fList[way] ) ;

    // print 使用的排程方法名稱
    fout << Wayname(way) << endl ;

    // print 甘特圖
    if ( way == 7 ) {  // 如果是all，就把所有的方法的甘特圖都印出來
        for ( int i = 1 ; i < gantte.size() ; i++ ) {
            fout << "==        " << Wayname(i) << "==" << endl ;
            for ( int j = 0 ; j < gantte[i].size() ; j++ ) {
                fout << gantte[i][j] ;
            } // for 
            fout << endl ;
        } //  for
    } // if 
    else {
        fout << "==        " << Wayname(way) << "==" << endl ;
        for ( int i = 0 ; i < gantte[way].size() ; i++ ) {
            fout << gantte[way][i] ;
        } // for 
        fout << endl ;
    } // else 
    
    fout << "===========================================================" << endl << endl ;

    // print waiting time
    fout << "Waiting Time" << endl ;

    if ( way == 7 ) {
        fout << "ID	FCFS	RR	SJF	SRTF	HRRN	PPRR" << endl ;
        fout << "===========================================================" << endl ;
        for ( int i = 0 ; i < fList[1].size() ; i++ ) {
            fout << fList[1][i].PID ;
            for ( int way = 1 ; way < 7 ; way++ ) {
                fout << '\t' << fList[way][i].wait ;  
            } // for ()
            fout << endl ;   
        } // for
    } // if 
    else {
        fout << "ID" << '\t'  << Wayname(way) << endl ;
        fout << "===========================================================" << endl  ;
        for ( int i = 0 ; i < fList[way].size() ; i++ ) {
            fout << fList[way][i].PID << '\t' << fList[way][i].wait << endl ;     
        } // for
    } // else 

    fout << "===========================================================" << endl << endl ;

    // print Turnaround time
    fout << "Turnaround Time" << endl ;
    if ( way == 7 ) {
        fout << "ID	FCFS	RR	SJF	SRTF	HRRN	PPRR" << endl ;
        fout << "===========================================================" << endl  ;
        for ( int i = 0 ; i < fList[1].size() ; i++ ) {
            fout << fList[1][i].PID ;
            for ( int way = 1 ; way < 7 ; way++ ) {
                fout << '\t' << fList[way][i].tt ;  
            } // for ()
            fout << endl ;   
        } // for
    } // if 
    else {
        fout << "ID" << '\t'  << Wayname(way) << endl ;
        fout << "===========================================================" << endl  ;
        for ( int i = 0 ; i < fList[way].size() ; i++ ) {
            fout << fList[way][i].PID << '\t' << fList[way][i].tt << endl ;     
        } // for
    } // else   

    fout << "===========================================================" << endl << endl ;

} // Outputfile()


int main() {

    string filename ;
    Scheduler sch ;

    cout << "Please enter File Name (eg. input1) : " << endl ;
    cin >> filename ;  // get the file name

    if ( Readfile( filename ) ) {
        sch.manager( way, timeslice ) ;
        // cout << "success " << endl ;
        Outputfile( filename, way ) ;
    } // if 

    else cout << "Please try again." << endl << endl ;

    system("pause") ;
} // main()