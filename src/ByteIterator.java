import java.util.Arrays;
import java.util.List;
import java.util.ArrayList;

public class ByteIterator implements Runnable {
    // the subsection limits for this object
    private final Byte[] byteMin; // TODO: should these still be Byte not byte? and should these be volatile?
    private final Byte[] byteMax; //FIXME convert to byte[] 
    
    // the overal limit of the byte for all boards of this length
    private static final Byte realMin = 0;
    private static final int uRealMax = ((Board.getLength()-1) * 11 << 1) | 0b00000001;
    private static final Byte realMax = (byte) uRealMax;
    // FIXME: how does java evaluate finals with (hypotheticaly) non static equations in them?

    private volatile long shipCount;
    private Ship[] fleet;

    public ByteIterator(Byte[] byteMin, Byte[] byteMax, Ship[] fleet){
        this.byteMin = byteMin;
        this.byteMax = byteMax;
        this.fleet = fleet;
        shipCount=0;
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
        for (int i = 0; i < encodedBoard.length; i++) encodedBoard[i]=byteMin[i];

        int allBoards=0;
        int goodBoards=0;

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
        
        shipCount = goodBoards;
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

        if(endVal) return true;
        return false;
    }

    // ----  Helper misc
    public static int byteToUint(byte data){
        return (int) data & 0b11111111;
    }

    // BUG: runs out of memory (fast for big fleets)
    public static List<Byte[]> subdivideBytes(int byteArraySize, int subdivdeCount){

        List<Integer> intSubDiv = new ArrayList<>();
        int radix = uRealMax+1;
        int maxSegVal = (int) Math.pow(radix,byteArraySize)-1;
        int segmentSize = (int) Math.floor(maxSegVal/subdivdeCount);
        // System.out.println("segSize "+segmentSize+" maxSegVal "+maxSegVal);
        
        int runningTotal=0;
        int i = 0;
        while (runningTotal<maxSegVal){ // calcualtes the int value of each subdivision
            runningTotal=segmentSize*i;
            if (runningTotal>maxSegVal) runningTotal=maxSegVal;
            intSubDiv.add(runningTotal);
            i++;
        }
        // System.out.println("intSubDivs: "+intSubDiv.toString());

        // converts the int subDivs to byte arrays with the appropriate radix
        List<Byte[]> byteSubDiv = new ArrayList<>();
        for(Integer div: intSubDiv){
            byteSubDiv.add(convertDecimalToBaseX(div, radix, byteArraySize));
        }
        // System.out.print("byte subdivs:");
        // for(Byte[] sub : byteSubDiv) System.out.print(Arrays.toString(sub)+", ");

        return byteSubDiv;
    }


    private static Byte[] convertDecimalToBaseX(int num, int radix, int byteArraySize){
        int remainder;

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


    // Display a message, preceded by the name of the current thread
    public static void threadMessage(String message) {
        String threadName = Thread.currentThread().getName();
        System.out.format("%s: %s%n",threadName,message);
    }

    // ----  Getters
    public long getShipCount(){
        return shipCount;
    }
    
    @Override
    public String toString(){
        return "Start: "+Arrays.toString(byteMin)+" Stop: "+Arrays.toString(byteMax)+" (Ships "+shipCount+")\n\t real min, max: "+realMin+", "+realMax;
    }

}
