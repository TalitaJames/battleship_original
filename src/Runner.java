import java.util.List;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Scanner;
import java.util.Random;
import java.util.Collections;
import java.util.Set;
import java.util.HashSet;
import java.util.Map;
import java.util.HashMap;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.PrintWriter;
import java.io.FileNotFoundException;
import java.io.IOException;

import java.util.concurrent.CountDownLatch;

public class Runner {
    private static long[][] heatmap;

    public static void main(String[] args) {
        parseSettings(args, true);

        Worker eric = new Worker(GameState.getMinByteArray(), GameState.getMaxByteArray(), GameState.getFleet());
        long startTime_v1 = System.currentTimeMillis();    
        long boardCount_eric = eric.checkBoards();
        long endTime_v1 = System.currentTimeMillis();    

        // Get/Save data
        String workerFilename = "../out/serialisedData/"+Board.getLength()+"-"+GameState.getFleet().length+".ser";
        eric.serializeWorker(workerFilename);

        Worker fred = new Worker(Worker.deserializeWorker(workerFilename).configToSequence(), GameState.getFleet());
        long startTime_v2 = System.currentTimeMillis();    
        long boardCount_fred = fred.checkBoards();
        long endTime_v2 = System.currentTimeMillis();    
           
        appendToCSV("../timeTesting/results_shipCount.txt", "("+String.valueOf(boardCount_eric)+","+String.valueOf(boardCount_fred)+")");
        appendToCSV("../timeTesting/results_timeData_java.txt", "("+String.valueOf(endTime_v1 - startTime_v1)+","+String.valueOf(endTime_v2 - startTime_v2)+")");
    }

    // ---- Generating Board Obj Methods
    public static long createThreads(){

        // sets up the threads
        // ta (thread array), ba (ByteIterator array), tg (thread group), lg (lifes good)
        Thread[] ta = new Thread[GameState.getThreadCount()]; 
        ByteIterator[] ba = new ByteIterator[GameState.getThreadCount()];
        ThreadGroup tg = new ThreadGroup("Mission A");  

        CountDownLatch threadDoneSignal = new CountDownLatch(ta.length); // count of live threads

        long startTime = System.currentTimeMillis();    
        // create each thread for that subset of values
        for(int i=0; i<GameState.getThreadCount(); i++){
            ba[i] = new ByteIterator(GameState.getFleet(), threadDoneSignal);
            ta[i] = new Thread(tg,ba[i]);
            ta[i].start();
        }
        
        // Wait for all the threads to finish
        try {
            threadDoneSignal.await();
        } catch(InterruptedException e) {System.err.println("Uhoh! it got interupted?");}

        long endTime = System.currentTimeMillis();

        // all the threads are done, so sum their results
        long goodBoards = 0;
        for(int i=0; i<ba.length; i++){
            goodBoards+=ba[i].getShipCount();
        }

        // heatmap = ba[0].getHeatmap();
        // for(int i=1; i<ba.length; i++){
        //     for(int j = 0; j < heatmap.length; j++){
        //         for(int k = 0; k < heatmap.length; k++){
        //             heatmap[j][k] += ba[i].getHeatmap()[j][k];
        //         }
        //     }  
        // }

        System.out.println(goodBoards);
        
        // append status to file
        appendToCSV("../timeTesting/results_shipCount.txt", String.valueOf(goodBoards));
        appendToCSV("../timeTesting/results_timeData_java.txt", String.valueOf(endTime - startTime));

        return goodBoards;
    }

    // ---- IO Methods
    public static void appendToCSV(String filename, String value){
        File file = new File(filename);
		try (FileWriter fileWriter = new FileWriter(file, true)){			
			fileWriter.write(value+",");
		} catch (IOException e) {
			e.printStackTrace();
		}
    }

    private static void parseSettings(String[] args, boolean verbose) {
        int boardSize = 5; // default
        int threadCount = 1;
        Ship[] fleet;
    
        // no args, use defult paramaters and empty hitmask
        if (args.length == 0) {
            fleet = new Ship[2]; 
            fleet[0] = new Ship(3, '3');
            fleet[1] = new Ship(2, '2');
            
            GameState.setAll(fleet, threadCount, new HashMap<Integer,Boolean>()); 
            Board.setLength(boardSize);
        }

        // input is given in the form: boardSize fleetInfo threadCount
        // fleet is "[3:a, 2:z, 4:w]" (where its length:char) 
        else if(args.length == 3){ 

            // board length
            try {
                boardSize = Integer.parseInt(args[0]);
                if(0>=boardSize || boardSize>12) {
                    closeProgram("! Invalid input args ! (boardSize must be 0<size<13)");
                }
            } catch (NumberFormatException e) {
                closeProgram("! Invalid input args ! (boardSize must be an int)");
            }

            //fleetInfo
            try {
                String[] ships = args[1].replace("[", "").replace("]", "").split(",");
                fleet = new Ship[ships.length];
                for(int i=0; i<ships.length; i++){

                    ships[i] = ships[i].replaceAll("\\s", ""); // remove all whitespace
                    String[] ship = ships[i].split(":"); // split into length and char
                    fleet[i]=new Ship(Integer.parseInt(ship[0]), ship[1].charAt(0));
                }
                
                GameState.setFleet(fleet);
            } catch (Exception e) {
                closeProgram("! Invalid input args ! fleet must be \"[3:a, 2:z, 4:w]\" (where its length:char) ");
            }
            

            //threadCount
            try{
                threadCount = Integer.parseInt(args[2]);
                if(0>=threadCount) {
                    closeProgram("! Invalid input args ! (threadCount must be 0<threadCount)");
                }
            } catch(NumberFormatException e){
                closeProgram("! Invalid input args ! (threadCount must be an int)");
            }
            
            GameState.setThreadCount(threadCount);
            GameState.setHitmask(new HashMap<Integer,Boolean>()); 
            Board.setLength(boardSize);
        }
        
        // something else has gone wrong
        else{
            closeProgram("! Invalid input args ! (must have args: size [fleet] threadCount)");
        }
        
        // setup the jobQueue
        JobQueue.initJobQueue();

        // prints system status
        if(verbose){
            System.out.println("System running with: \n"+
                            "\tBoardSize: "+Board.getLength()+
                            "\tfleetSize: "+GameState.getFleet().length+
                            "\tthreadCount: "+GameState.getThreadCount());
        }
    }
    
    public static void closeProgram(){
        closeProgram("Fatal Error! Closing program!");
    }

    public static void closeProgram(String errorMsg){
        System.err.println(errorMsg);
        System.exit(0);
    }    
}
