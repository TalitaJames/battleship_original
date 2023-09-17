import java.util.List;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Scanner;
import java.util.Random;
import java.util.Collections;
import java.util.Set;
import java.util.HashSet;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.PrintWriter;
import java.io.FileNotFoundException;
import java.io.IOException;

import java.util.concurrent.CountDownLatch;

public class Runner {
    private static Ship[] fleet;
    private static int threadCount;
    
    private static long[][] heatmap;

    public static void main(String[] args) {
        parseSettings(args, false);
        // createThreads();

        // Re testing board & play
        Board foo = new Board();
        try{
            foo.placeShip(fleet[0],Board.coord(0,0),true);
            foo.placeShip(fleet[1],Board.coord(0,1),true);

            System.out.println(foo.displaySetup());
            System.out.println(foo);
            foo.attack(10);
            foo.attack(12);
            foo.attack(34);
            System.out.println(foo);
            System.out.println(foo.getHitmask());
        } catch (Exception e) {System.out.println("oops");}


        /* Worker testing mess
        byte absMin = 0;
        int uAbsMax = ((Board.getLength()-1) * 11 << 1) | 0b00000001;
        byte absMax = (byte) uAbsMax;
        
        Byte[] start = new Byte[fleet.length];
        Byte[] end = new Byte[fleet.length];

        for(int i=0; i<fleet.length; i++){
            start[i]=absMin;
            end[i]=absMax;
        }


        String filename = "../out/eric.ser";

        Worker eric = new Worker(start, end, fleet);

        long ericBoards = eric.doThing();
        System.out.println(ericBoards);
        // eric.testing();
        List<Byte[][]> segments = eric.configToSequence();
        eric.serializeWorker(filename);
        */

        

    }

    // ---- Generating Board Obj Methods
    public static long createThreads(){

        // sets up the threads
        // ta (thread array), ba (ByteIterator array), tg (thread group), lg (lifes good)
        Thread[] ta = new Thread[threadCount]; 
        ByteIterator[] ba = new ByteIterator[threadCount];
        ThreadGroup tg = new ThreadGroup("Mission A");  

        CountDownLatch threadDoneSignal = new CountDownLatch(ta.length); // count of live threads

        long startTime = System.currentTimeMillis();    
        // create each thread for that subset of values
        for(int i=0; i<threadCount; i++){
            ba[i] = new ByteIterator(fleet, threadDoneSignal);
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
        String timeCSV = (endTime - startTime)+",";
        String countCSV = goodBoards+",";

        File fTime = new File("../timeTesting/results_timeData_java.txt");
        File fCount = new File("../timeTesting/results_shipCount.txt");
		try (FileWriter frTime = new FileWriter(fTime, true);
             FileWriter frCount = new FileWriter(fCount, true)){			
			frTime.write(timeCSV);
            frCount.write(countCSV);
		} catch (IOException e) {
			e.printStackTrace();
		}
        return goodBoards;
    }

    // ---- IO Methods
    private static void parseSettings(String[] args, boolean verbose) {
        int boardSize = 5; // default
        threadCount = 1;
       
        // no args, use defult paramaters
        if (args.length == 0) {
            fleet = new Ship[2]; 
            fleet[0] = new Ship(3, '3');
            fleet[1] = new Ship(2, '2');
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
                
                Board.setLength(boardSize);
            } catch (Exception e) {
                closeProgram("! Invalid input args ! fleet must be \"[3:a, 2:z, 4:w]\" (where its length:char) ");
            }

            //threadCount
            try{
                threadCount = Integer.parseInt(args[2]); //FIXME: implement error checking later (& move to parse?)
                if(0>=threadCount) {
                    closeProgram("! Invalid input args ! (threadCount must be 0<threadCount)");
                }
            } catch(NumberFormatException e){
                closeProgram("! Invalid input args ! (threadCount must be an int)");
            }
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
                            "\tfleetSize: "+fleet.length+
                            "\tthreadCount: "+threadCount);
        }

    }
    
    public static void closeProgram(){
        closeProgram("Fatal Error! Closing program!");
    }

    public static void closeProgram(String errorMsg){
        System.err.println(errorMsg);
        System.exit(0);
    }
   
    
    public static Ship[] getFleet(){
        return fleet;
    }

    
}
