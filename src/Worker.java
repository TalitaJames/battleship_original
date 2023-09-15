import java.util.List;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Set;
import java.util.HashSet;

public class Worker{

    private final Ship[] fleet;

    // configuration sequence unwrapped [start, end)
    private Byte[] minByte;
    private Byte[] maxByte;

    // Absolute limit variables for this board length
    private static final byte absMin = 0;
    private static final int uAbsMax = ((Board.getLength()-1) * 11 << 1) | 0b00000001;
    private static final byte absMax = (byte) uAbsMax;
    


    public Worker(Byte[] minByte, Byte[] maxByte, Ship[] fleet){
        this.minByte=minByte;
        this.maxByte=maxByte;
        this.fleet=fleet;
    }
    
    
    // Given a config sequence, 



    // Do: Given a config sequence segment [start byte, end byte) check each byte, and make note of when it changes state (from good to bad)
    public long doThing(){ //FIXME: rename 
        boolean stateIsBad = false; // state of segment being explored
        
        List<Byte[]> config = new ArrayList<>();

        // while (go thru all the bytes specified in this subgroup)
            
        long goodBoards =0;
        Byte[] runnerByte = Arrays.copyOf(minByte, minByte.length); // the byte that iterates thru each 
        PrimitiveBoard trialBoard = new PrimitiveBoard(runnerByte, fleet);
        while(runnerByte!=null){
            trialBoard = new PrimitiveBoard(runnerByte, fleet);
            if(trialBoard != null && !trialBoard.getIsBad()){
                goodBoards++;
            }

            if(stateIsBad != trialBoard.getIsBad()){
                stateIsBad=trialBoard.getIsBad();
                config.add(Arrays.copyOf(runnerByte, runnerByte.length));                
            }
            runnerByte = nextByte(runnerByte); 
        }

        System.out.println("");
        for (Byte[] printMe : config) System.out.print(Arrays.toString(printMe)+", ");
        System.out.println(config.size());
        return (long) goodBoards;
    }

    // Split


    // Combine


    // Count: Given a config sequence, return int (number of boards it contains)
    // If [S,E) then 


    private Byte[] nextByte(Byte[] data){
        boolean validByte = false;
        
        while(!validByte){
            for (int i = 0; i < data.length; i++) {
                if (data[i]==absMax){
                    data[i]=absMin;
                } else{
                    data[i]++;
                    break;
                }
            }

            // checks if is a valid byte[] (ie all bytes are unique and none are illegal)
            Set<Byte> ByteSet = new HashSet<>();
            // for (Byte t : data) ByteSet.add((byte) (t/2));
            // if (ByteSet.size() == data.length) validByte = true;

            validByte = true;
            
            if(checkEndVal(data)) return null;
        }
        return data;
    }

    private boolean checkEndVal(Byte[] data){ // checks if at the end value of the sub list to check
        boolean endVal = true;
        int n = 0;
        while(endVal && n<data.length){
            endVal = (data[n]==maxByte[n]);
            n++;
        }
        return endVal;
    }

}