import java.util.Arrays;
import java.util.List;
import java.util.ArrayList;
import java.util.concurrent.CountDownLatch;

public class ByteIterator implements Runnable {
    private volatile long shipCount;
    private long[][] heatmap;
    private Ship[] fleet;

    private final CountDownLatch threadDoneSignal; //the count for the threads to halt main

    public ByteIterator(Ship[] fleet, CountDownLatch threadDoneSignal){ //}, ReentrantLock lock){
        shipCount=0;
        this.fleet = fleet;
        this.threadDoneSignal = threadDoneSignal;
    }  

    @Override
    public void run(){
        shipCount+=runJobQueue();
        threadDoneSignal.countDown();
    }

    
    // ---- Job Queue
    private long runJobQueue(){
        long goodBoards=0;
        
        Byte[] trialByte = JobQueue.getByte();

        while(trialByte != null){
            PrimitiveBoard trialBoard = new PrimitiveBoard(trialByte, fleet);
            if(trialBoard != null && !trialBoard.getIsBad()) {
                goodBoards++;      
                // boolean[][] trialheatmap = trialBoard.returnHeatmap();
                // for(int j = 0; j < heatmap.length; j++){
                //     for(int k = 0; k < heatmap.length; k++){
                //         heatmap[j][k] += trialheatmap[j][k] ? 1:0;
                //     }
                // }
            }
            trialByte = JobQueue.getByte();
        }
        return goodBoards;
    }


    // Display a message, preceded by the name of the current thread
    public static void threadMessage(String message) {
        String threadName = Thread.currentThread().getName();
        System.out.format("%s: %s%n",threadName,message);
    }

    // ----  Getters
    public long getShipCount(){
        return shipCount; // this means board count, should rename
    }

    public long[][] getHeatmap(){
        return heatmap;
    }
    
    @Override
    public String toString(){
        return Thread.currentThread().getName()+" (Ships "+shipCount+")";
    }

}
