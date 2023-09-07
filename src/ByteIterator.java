import java.util.Arrays;
import java.util.List;
import java.util.ArrayList;
import java.util.concurrent.locks.ReentrantLock;

public class ByteIterator implements Runnable {
    private volatile long shipCount;
    private Ship[] fleet;
    private static final ReentrantLock lock = new ReentrantLock();


    public ByteIterator(Ship[] fleet){ //}, ReentrantLock lock){
        shipCount=0;
        this.fleet = fleet;
    }  

    @Override
    public void run(){
        shipCount+=runJobQueue();
        // ByteIterator.threadMessage("I'm Starting!");
        // shipCount+=iterateBytesPrimitive(this.fleet); 
        // ByteIterator.threadMessage("I'm Done!");
    }

    
    // ---- Job Queue
    public long runJobQueue(){
        long goodBoards=0;
        
        Byte[] foo = JobQueue.getByte();

        while(foo != null){
            PrimitiveBoard test = new PrimitiveBoard(foo, fleet);
            if(test != null && !test.getIsBad()) goodBoards++;      

            foo = JobQueue.getByte();
        }

        return goodBoards;
    }


    /*
    // ---- Generating Board Byte Methods
    private long iterateBytes(Ship[] fleet){
        
        Byte[] encodedBoard = new Byte[fleet.length];
        for (int i = 0; i < encodedBoard.length; i++) encodedBoard[i]=byteMin[i];

        long allBoards=0;
        long goodBoards=0;

        while(encodedBoard!=null){
            // if(allBoards%1e7==0) ByteIterator.threadMessage(Arrays.toString(encodedBoard));
            try {
                Board test = Board.decodeBoard(encodedBoard, fleet);
                goodBoards++;
            } catch (InvalidIntersectionException e){
                // System.err.print(" Intersection");
            } catch (InvalidPlacementException e) {
                // System.err.print(" Placement");
            } catch (InvalidShipTypeException e) {
                System.err.println("Uhoh! Ship Type is wrong");
                System.err.println(e.getStackTrace());
            } catch (InvalidPositionException e){
                // System.err.println("Uhoh: Bad coordinate");
            }

            allBoards++;
            encodedBoard = nextByte(encodedBoard); 
        }
        
        return (long) goodBoards;
    }

    private long iterateBytesPrimitive(Ship[] fleet){
        
        Byte[] encodedBoard = new Byte[fleet.length];
        for (int i = 0; i < encodedBoard.length; i++) encodedBoard[i]=byteMin[i];

        long allBoards=0;
        long goodBoards=0;

        while(encodedBoard!=null){
            PrimitiveBoard test = new PrimitiveBoard(encodedBoard, fleet);
            if(test != null && !test.getIsBad()) goodBoards++;

            allBoards++;
            encodedBoard = nextByte(encodedBoard); 
        }
        
        return (long) goodBoards;
    }


    private Byte[] nextByte(Byte[] data){
        boolean validByte = false;
        
        while(!validByte){
            // iterate the value
            for (int i = 0; i < data.length; i++) {
                if (data[i]==realMax){
                    data[i]=realMin;
                } else{
                    data[i]++;
                    break;
                }
            }

            // checks if is a valid byte[] (ie all bytes are unique and none are illegal)
            if(!(Arrays.stream(data).distinct().count() < data.length)) validByte = true;
            if(checkEndVal(data)) return null;
        }
        return data;
    }

    private boolean checkEndVal(Byte[] data){ // checks if at the end value of the sub list to check
        boolean endVal = true;
        int n = 0;
        while(endVal && n<data.length){
            endVal = (data[n]==byteMax[n]);
            n++;
        }
        return endVal;
    }

    // ----  Helper misc
    public static int byteToUint(byte data){
        return (int) data & 0b11111111;
    }

    public static List<Byte[]> subdivideBytes(int byteArraySize, int subdivideCount){
        // List<Long> base10SubDiv = new ArrayList<>(); // for plotting and checking results, not needed unless debuging
        List<Byte[]> byteSubDiv = new ArrayList<>();

        int radix = uRealMax+1;
        long maxSegVal = (long) Math.pow(radix,byteArraySize)-1;
        long segmentSize = (long) Math.floor(maxSegVal/subdivideCount);
        System.out.println("\tSegSize: "+segmentSize+"\tmaxSegVal: "+maxSegVal);
        
        long runningTotal=0;
        int i = 0;
        while (runningTotal<maxSegVal){ // calcualtes the int value of each subdivision
            runningTotal=segmentSize*i;
            if (runningTotal>maxSegVal) runningTotal=maxSegVal;
            // base10SubDiv.add(runningTotal);
            byteSubDiv.add(convertDecimalToBaseX(runningTotal, radix, byteArraySize));
            i++;
        }
        return byteSubDiv;
    }


    private static Byte[] convertDecimalToBaseX(long num, int radix, int byteArraySize){
        long remainder;

        Byte[] converted = new Byte[byteArraySize];
        for (int i = 0; i < converted.length; i++) converted[i]=0;

        int i = 0;

        while (num > 0) {
            remainder = num % radix;
            num /= radix;
            
            converted[i]= (byte)remainder;
            i++;
        }

        // System.out.println(Arrays.toString(converted)+"\n");

        return converted;
    }
    */

    // Display a message, preceded by the name of the current thread
    public static void threadMessage(String message) {
        String threadName = Thread.currentThread().getName();
        System.out.format("%s: %s%n",threadName,message);
    }

    // ----  Getters
    public long getShipCount(){
        return shipCount; // this means board count, should rename
    }
    
    @Override
    public String toString(){
        // return "Start: "+Arrays.toString(byteMin)+" Stop: "+Arrays.toString(byteMax)+" (Ships "+shipCount+")\n\t real min, max: "+realMin+", "+realMax;
        return " (Ships "+shipCount+")";
    }

}
