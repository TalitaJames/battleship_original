import java.util.List;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Set;
import java.util.HashSet;
import java.util.Map;
import java.util.HashMap;

import java.io.File;
import java.io.FileOutputStream;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.ObjectOutputStream;
import java.io.ObjectInputStream;
import java.io.Serializable;
import java.io.IOException;

public class Worker implements Serializable {

    private final Ship[] fleet; // can maybe be static between workers?

    // configuration unwrapped (Should it be [start, end)? currently [start, end]  ) 
    private Byte[] minByte;
    private Byte[] maxByte;

    // Absolute limit variables for this board length
    private static final byte absMin = 0;
    private static final int uAbsMax = ((Board.getLength()-1) * 11 << 1) | 0b00000001;
    private static final byte absMax = (byte) uAbsMax;

    private List<Byte[]> configByte;
    private List<Byte[]> configBool;

    // Things only for bosses
    private List<Worker> subordinates; 

    //TODO: A list of bytes should make a list of workers from the config
    // should have a list of childrenWorkers (subordinates) but need a new name?
    // i don't think it needs to know if it is a child vs an independant worker
    
    // When threading
    // All workers should accept a single countdown latch 
    // (if it is from a parent then it will have been initalized at a higher value than as a solo)
    // the runner has 1 and given to boss worker 
    // boss will make one for n=subordinate and they all recive.
    // subordinates don't know about coworkers, just their job
    // boss knows about subordinates, they don't know about her though

    // --- Constructors and Factory methods
    public Worker(Byte[] minByte, Byte[] maxByte, Ship[] fleet){
        this.minByte=minByte;
        this.maxByte=maxByte;
        this.fleet=fleet;

        configByte = new ArrayList<>();
        // configBool = new ArrayList<>();
        
        subordinates = new ArrayList<>();
        subordinates.add(this);
    }

    public Worker(List<Byte[][]> sequence, Ship[] fleet){
        subordinates = new ArrayList<>();

        for(int i=0; i<sequence.size(); i++){
            Worker newHire = new Worker(sequence.get(i), fleet);
            subordinates.add(newHire);
        }

        this.fleet=fleet;

        // this.minByte=minByte; //FIXME: why doesn't the compiler complain about this not being initalised?
        // this.maxByte=maxByte;

        // configByte = new ArrayList<>();
        // configBool = new ArrayList<>();
    }

    public Worker(Byte[][] startStop, Ship[] fleet){
        this(startStop[0], startStop[1], fleet);
    }

    public long checkBoards(){
        long total=0;
        int i=0;
        for (Worker eric: subordinates){
            total+=eric.checkBoards_internal();
        }
        return total;
    }

    // Do: Given a config sequence segment check each byte, and make note of when it changes state (from good to bad)
    private long checkBoards_internal(){
        boolean stateIsBad = false; // state of segment being explored
            
        long goodBoards = 0; 
        Byte[] runnerByte = Arrays.copyOf(minByte, minByte.length); // the byte that iterates thru each board
        PrimitiveBoard trialBoard = new PrimitiveBoard(runnerByte, fleet); // the board made
        int progression = 0;
        while(runnerByte!=null){
            if (++progression%5e8==0) System.out.println("\t"+Arrays.toString(runnerByte));

            trialBoard = new PrimitiveBoard(runnerByte, fleet);
            if(trialBoard != null && !trialBoard.getIsBad()) goodBoards++;

            if(stateIsBad != trialBoard.getIsBad()){
                Byte[] copiedByte = Arrays.copyOf(runnerByte, runnerByte.length);
                configByte.add(copiedByte); // add the change of state to the configuration
                // configBool.add(trialBoard.getIsBad());

                stateIsBad=trialBoard.getIsBad();
            }
            runnerByte = nextByte(runnerByte); 
        }
        //TODO: in the future, this will be threaded, so check for overlapping config information when that happens 
        // and the configBool needs to alternate TFTFTFTF 
        // now return the config and die(?)

        return goodBoards;
    }

    // Turns the configBytes from 
    public List<Byte[][]> configToSequence(){
        boolean switchState = false; // the state (false) that the board is looking for
        
        List<Byte[][]> sequence = new ArrayList<>();
    
        for(int i=1; i<configByte.size(); i+=2){
            Byte[][] singleConfig = new Byte[2][configByte.get(i).length];
            singleConfig[0] = configByte.get(i);  // starting segment
            singleConfig[1] = configByte.get(i+1);  // end segment
            sequence.add(singleConfig);
        }
        
        return sequence;
    }


    // Split


    // Combine


    // Count: Given a config sequence, return int (number of boards it contains)
    // If [S,E) then 

    // --- Byte Iteration Methods
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
            for (Byte t : data) ByteSet.add((byte) (t/2));
            if (ByteSet.size() == data.length) validByte = true;

            if(checkEndVal(data)) return null;
        }
        return data;
    }

    private boolean checkEndVal(Byte[] data){ // checks if at the end value of the sub list to check
        // boolean equal = true;
        // int n = 0;
        // while(equal && n<data.length){
        //     equal = (data[n]==maxByte[n]);
        //     n++;
        // }
        // return equal;

        return Arrays.equals(data, maxByte);
        // FIXME: Why does the latter work for everything, but the former doesn't work on serialised data?
        // Also how do serialised objs work on change of methods but not data? (how do they store data)
    }

    // --- IO
    public void serializeWorker(String filename){
        try {
            FileOutputStream fileOut = new FileOutputStream(filename);
            ObjectOutputStream out = new ObjectOutputStream(fileOut);
            out.writeObject(this);
            out.close();
            fileOut.close();
            // System.out.println("Serialized data is saved in " + filename);
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public static Worker deserializeWorker(String filename){
        Worker tmp = null;
        try {
            FileInputStream fileIn = new FileInputStream(filename);
            ObjectInputStream in = new ObjectInputStream(fileIn);
            tmp = (Worker) in.readObject();
            in.close();
            fileIn.close();
        } catch (IOException i) {
            i.printStackTrace();
        } catch (ClassNotFoundException c) {
            System.out.println("Class not found");
            c.printStackTrace();
        }
        return tmp;
    }

    @Override
    public String toString(){
        String maxStr = maxByte != null ? Arrays.toString(maxByte) : "null";
        String minStr = minByte != null ? Arrays.toString(minByte) : "null";
        String subStr = subordinates != null ? String.valueOf(subordinates.size()) : "null";
        String configStr = configByte != null ? String.valueOf(configByte.size()) : "null";

        return "[S,E]: ["+minStr+", "+maxStr+"]\n"+subStr+" underlings & ConfigBytes "+configStr;
    }

    // --- Getters and Setters



    // --- Helper functions
    public static long byteToLong(Byte[] data){
        if (data==null) return -1;

        long index = 0;

        for (int i = data.length-1; i >= 0; i--) {
            index += data[i]*Math.pow(uAbsMax+1,i);
        }
        return index;
    }

    public static Byte[] longToByte(long data){
        if (data < 0 || data > byteToLong(GameState.getMaxByteArray())) return null;

        Byte[] index = new Byte[GameState.getFleet().length];
        for (int i = 0; i < index.length; i++) {
            index[i] = (byte) (data % (uAbsMax+1));
            data /= (uAbsMax+1);
        }
        return index;
    }
}