import java.util.Arrays;
import java.util.Set;
import java.util.HashSet;

public class JobQueue{
    private static volatile Byte[] nextByte;
    private static final Object lock = new Object();

    public static void initJobQueue(){
        nextByte = new Byte[GameState.getFleet().length];
        for (int i=0; i<nextByte.length; i++) nextByte[i]=GameState.getMinByte();
    }

    public synchronized static Byte[] getByte(){
        Byte[] currentByte;
        synchronized(lock){
            boolean validByte = false; 
            if(nextByte==null || checkEndVal(nextByte)) return null;

            // Byte[] currentByte = nextByte;
            currentByte = Arrays.copyOf(nextByte, nextByte.length);
        
            while(!validByte && nextByte!=null){
                // iterate the value
                for (int i = 0; i < nextByte.length; i++) {
                    if (nextByte[i]==GameState.getMaxByte()){
                        nextByte[i]=GameState.getMinByte();
                    } else{
                        nextByte[i]++;
                        break;
                    }
                }

                // checks if is a valid byte[] (ie all bytes are unique and none are illegal)
                Set<Byte> ByteSet = new HashSet<>();
                for (Byte t : nextByte) ByteSet.add((byte) (t/2));
                if (ByteSet.size() == nextByte.length) validByte = true;

                if(checkEndVal(nextByte)) nextByte=null;
            }
        }
        return currentByte;
    }

    private static boolean checkEndVal(Byte[] data){ // checks if reached the end of all bytes to check
        boolean endVal = true;
        int n = 0;
        while(endVal && n<data.length){
            endVal = (data[n]==GameState.getMaxByte());
            n++;
        }
        return endVal;
    }
}

