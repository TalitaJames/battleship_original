import java.util.Arrays;
import java.util.List;
import java.util.ArrayList;

public class ByteIterator implements Runnable {
    // the subsection limits for this object
    private final Byte byteMin; // TODO: should these still be Byte not byte? and should these be volatile?
    private final Byte byteMax;
    
    // the overal limit of the byte for all boards of this length
    private static final Byte realMin = 0;
    private static final int uRealMax = ((Board.getLength()-1) * 11 << 1) | 0b00000001;
    private static final Byte realMax = (byte) uRealMax;
    // FIXME: how does java evaluate finals with (hypotheticaly) non static equations in them?

    private volatile long shipCount;
    private Ship[] fleet;

    public ByteIterator(Byte byteMin, Byte byteMax, Ship[] fleet){
        this.byteMin = byteMin;
        this.byteMax = byteMax;
        this.fleet = fleet;
    }  

    @Override
    public void run(){
        // ByteIterator.threadMessage("I'm Starting!");
        iterateBytes(this.fleet); 
        // ByteIterator.threadMessage("I'm Done!");
    }


    // ---- Generating Board Byte Methods
    private long iterateBytes(Ship[] fleet){
        
        Byte[] encodedBoard = new Byte[fleet.length];
        for (int i = 0; i < encodedBoard.length; i++) encodedBoard[i]=byteMin;

        int allBoards=0;
        int goodBoards=0;

        while(encodedBoard!=null){
            // if(allBoards%1e7==0) ByteIterator.threadMessage(Arrays.toString(encodedBoard));
            // FIXME: there is an error that the some bytes get checked twice when the threads are divided (because one ends on [4,4] and the next starts [4,4])
            // this doesn't cause issues above ship counts of 1, (because they are known to intersect, and would be skipped by `nextByte()` anyway)
            // in ship lengths of 1 this is an issue
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
        
        shipCount = goodBoards;
        return (long) goodBoards;
    }


    private Byte[] nextByte(Byte[] data){
        boolean validByte = false;
        
        while(!validByte){
            // Check if at the end of the values
            boolean endVal = true;
            int n = 0;
            while(endVal && n<data.length){
                endVal = (data[n]==byteMax);
                n++;
            }
            if(endVal) return null;

       
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
        }
    
        return data;
    }

    // ----  Helper misc
    public static int byteToUint(byte data){
        return (int) data & 0b11111111;
    }

    public static byte[][] subdivideBytes(int byteArraySize, int subdivdeCount){

        // System.out.println(convertDecimalToBaseX(7,2));

        List<Integer> intSubDiv = new ArrayList<>();

        int maxVal = (int) Math.pow(uRealMax,byteArraySize);
        int segmentSize = (int) Math.floor(maxVal/subdivdeCount);
        // System.out.println("segSize "+segmentSize+" maxVal "+maxVal);
        
        int runningTotal=0;
        int i = 0;
        while (runningTotal<maxVal){
            runningTotal=segmentSize*i;
            if (runningTotal>maxVal) runningTotal=maxVal;
            // System.out.println("\trun "+runningTotal+" i"+i);
            intSubDiv.add(runningTotal);
            i++;
        }
        System.out.println(intSubDiv.toString());



        // int remainder;
        // int radix = uRealMax;
        
        // int byteLoc = byteArraySize-1;
        // int[] foo = new int[byteArraySize];

        // System.out.println("\tsegSize "+segmentSize+" maxUbyte "+uRealMax);
        // while(segmentSize>0){
        //     remainder = segmentSize % radix;
        //     foo[byteLoc]=remainder;
        //     segmentSize /= radix;

        //     System.out.println("rem: "+remainder+" seg left:"+segmentSize);
        //     System.out.println("\t"+Arrays.toString(foo));
        //     byteLoc--;
        // }

        // System.out.println(Arrays.toString(foo));





        // int segmentSize = (int) Math.floor(uRealMax/subdivdeCount);
        
        // System.out.println("byteArraySize "+byteArraySize+" subDev "+subdivdeCount);


        // double remainingSegments=segmentSize;

        // for(int i=byteArraySize-1; i>=0;i--){

        //     int currSize = (int)Math.floor(remainingSegments/(Math.pow(uRealMax,i)));
        //     foo[i] = (byte) currSize;

        //     remainingSegments = (double) remainingSegments%(Math.pow(uRealMax,i));

        //     System.out.println(i+" currSize: "+currSize+" rem:"+remainingSegments+" "+Arrays.toString(foo));
        // }

        // // byte[][] bar = new byte[][]

        return new byte[subdivdeCount][byteArraySize];
    }



    private static String convertDecimalToBaseX(int num, int radix){
        String result = "";

        int remainder;

        while (num > 0) {
            remainder = num % radix;
            result += remainder;
            num /= radix;
            System.out.println("rem: "+remainder+" result: "+result+" num:"+num);
        }
        return new StringBuffer(result).reverse().toString();
    }


    // Display a message, preceded by the name of the current thread
    public static void threadMessage(String message) {
        String threadName = Thread.currentThread().getName();
        System.out.format("%s: %s%n",threadName,message);
    }

    // ----  Getters
    public long getShipCount(){
        return shipCount;
    }

}
