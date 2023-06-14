// import java.util.Arrays;
// import java.util.ArrayList;
import java.util.List;
import java.util.BitSet;


public class Runner {

    public static int BOARD_LENGTH;
    public static int[] shipSizes = {3,2};
    
    public static void setValues() {
        BOARD_LENGTH=5;
    }

    // i'm sure there's a more efficient way to do this (and i think the way is to not use a 2d array)
    public static boolean[][] transpose(boolean inital[][]) {
        boolean transposed[][] = new boolean[inital[0].length][inital.length];
    
        for (int i = 0; i < inital[0].length; i++) {
            for (int j = 0; j < inital.length; j++) {
                transposed[i][j] = inital[j][i];
            }
        }
        return transposed;
    }


    // i think this is equivilant to the `positions()` python code in `yrs_battleship.py` (with flatten=true)
    public static List<boolean[][]> positions(int shipLength) {
        
        int singleRowPlacements = (BOARD_LENGTH-shipLength+1);
        int placements = 2*BOARD_LENGTH*singleRowPlacements; // the # of possible possitions for the ship to be in the grid
        
        // all the different single row arangements 
        List<boolean[]> singleRowlocations = new ArrayList<boolean[]>();
        for (int i = 0; i < singleRowPlacements; i++) {
            boolean[] singleRow = new boolean [BOARD_LENGTH];
            Arrays.fill(singleRow, i, i+shipLength, true);
            singleRowlocations.add(singleRow);
        }
        
        List<boolean[][]> locations = new ArrayList<boolean[][]>(); // list to be filled w/all legal ship positions 

        // horizontal positions
        for (int i = 0; i < BOARD_LENGTH; i++) {
            for (int j = 0; j < singleRowlocations.size(); j++) {
                boolean[][] location = new boolean [BOARD_LENGTH][BOARD_LENGTH];
                location[i]=singleRowlocations.get(j);

                locations.add(location);
            }   
        }

        // vertical positions (the transposed of all the possible horizontal pos)
        for (int i = 0; i < placements/2; i++) {
            // placements doesn't have to be used here, and i think it makes more sense to save the current size of locations and base it of that
            locations.add(transpose(locations.get(i)));
        }

        return locations;
    } 


    public static List<BitSet[]> positions_bitSet(int shipLength) {
        
        int singleRowPlacements = (BOARD_LENGTH-shipLength+1);
        int placements = 2*BOARD_LENGTH*singleRowPlacements; // the # of possible possitions for the ship to be in the grid
        
        // all the different single row arangements 
        List<BitSet> singleRowlocations = new ArrayList<>();
        for (int i = 0; i < singleRowPlacements; i++) {
            BitSet singleRow = new BitSet (BOARD_LENGTH);

            singleRow.set(i,i+shipLength,true);

            singleRowlocations.add(singleRow);
        }
        
        List<BitSet[]> locations = new ArrayList<BitSet[]>(); // list to be filled w/all legal ship positions 

        // horizontal positions
        for (int i = 0; i < BOARD_LENGTH; i++) {
            for (int j = 0; j < singleRowlocations.size(); j++) {
                BitSet[] location = new BitSet[BOARD_LENGTH];
                location[i]=singleRowlocations.get(j);

                locations.add(location);
            }   
        }

        // vertical positions (the transposed of all the possible horizontal pos)
        for (int i = 0; i < placements/2; i++) {
            // placements doesn't have to be used here, and i think it makes more sense to save the current size of locations and base it of that
            // locations.add(transpose(locations.get(i)));
            locations.add(boolToBit(transpose(bitToBool(locations.get(i))))); //FIXME this is a terribly janky workarounds
        }

        return locations;

    } 



    // if the two booleans intersect, return true
    public static boolean intersection(boolean[][] posA, boolean[][] posB) {
        // assuming the two arrays are == size 
        for (int i = 0; i < posA.length; i++) {
            for (int j = 0; j < posA[i].length; j++) {
                if (posA[i][j]==true && posB[i][j]==true) {
                    return true;
                }
            }
        }
        return false;
    }

    public static boolean intersection(BitSet[] bitA, BitSet[] bitB) {
        // assuming the two arrays are == size 
        for (int i = 0; i < bitB.length; i++) {
            if (bitA[i]==null && bitB[i]==null) {
                // pass
            }
            else if ((bitA[i]==null && bitB[i]!=null) || (bitA[i]!=null && bitB[i]==null)) {
                return true;
            }
            else if(bitA[i].intersects(bitB[i])){
                return true;
            }
        }
   
        return false;
    }

    // it was here that i realised BitSets were overcomplicating this solution
    public static BitSet[] mergePosition(BitSet[] bitA, BitSet[] bitB) {
        BitSet[] merged = new BitSet[BOARD_LENGTH];
        // BitSet mergedL = new BitSet(BOARD_LENGTH);


        for (int i = 0; i < bitA.length; i++) {
            if  (bitA[i]!=null){
                merged[i]= (BitSet) bitA[i].clone();
            }
            else{
                merged[i]= new BitSet(BOARD_LENGTH);
                // System.err.println("OH NO, A is null");
            }
            if (bitB[i]!=null){
                merged[i].and(bitB[i]);
            }
        }
        // boolean merged[][] = new boolean[posA[0].length][posA.length];
        return merged;
    }

    

    // all of the printing things:
    public static void nicePrint(boolean[][] ship) {
        System.out.println("----boolean ship----");
        for (boolean[] shipRow : ship) {
            System.out.print("[ ");
            for (boolean cellValue : shipRow) {
                char symbol ='.';
                if(cellValue){
                    symbol='X';
                }
                System.out.print(symbol+" ");
                
            }
            System.out.println("]");
        }
    }

    public static void nicePrint(boolean[][] shipA, boolean[][] shipB) {
        System.out.println("----ship a----  ----ship b----");
        for (int i=0; i<shipA.length; i++) {
            System.out.print("[ ");
            for (boolean cellValue : shipA[i]) {
                char symbol ='.';
                if(cellValue){
                    symbol='X';
                }
                System.out.print(symbol+" ");
                
            }
            System.out.print("]\t[ ");
            for (boolean cellValue : shipB[i]) {
                char symbol ='.';
                if(cellValue){
                    symbol='X';
                }
                System.out.print(symbol+" ");
                
            }
            System.out.println("]");

        }
    }

    public static void nicePrint(BitSet[] ship) {
        System.out.println("---BitSet ship----");
        for (BitSet shipRow : ship) {
            System.out.print("[ ");
            for (int i = 0; i < ship.length; i++) {
                char symbol ='.';
                boolean cellValue = shipRow.get(i);
                
                if(cellValue){
                    symbol='X';
                }
                System.out.print(symbol+" ");
                
            }
            System.out.println("]");
        }
    }

    // i think some kind of binary aproach might be the way to go? for quick ANDing to check intersections ect?
    public static BitSet[] boolToBit(boolean[][] convert) {
        BitSet[] bitArray = new BitSet[convert.length];

        for (int i = 0; i < convert.length; i++) {
            BitSet newBitSet = new BitSet(convert.length);
           
            for (int j = 0; j < convert[i].length; j++) {
                newBitSet.set(j, convert[i][j]);
            }
            bitArray[i] = newBitSet;
        }

        return bitArray;
    }


    public static boolean[][] bitToBool(BitSet[] convert) {
        boolean[][] boolArray = new boolean[convert.length][convert.length];

        for (int i = 0; i < convert.length; i++) {
            boolean[] newBoolArray = new boolean[convert.length];

            if(convert[i]==null){ // null means row is empty
                Arrays.fill(newBoolArray, 0, convert.length, false);
            }else{
                    for (int j = 0; j < convert.length; j++) {
                        newBoolArray[j]=convert[i].get(j);
                    }
                }

            boolArray[i] = newBoolArray;
        }

        return boolArray;
    }


    public static void main(String[] args){
        setValues();

        long totalPos=1200; //TODO: only a const for 5x5(2,3)

        // List<boolean[][]> singleShipPos = positions(2);
        List<BitSet[]> singleShipPos_bit2 = positions_bitSet(2);
        List<BitSet[]> singleShipPos_bit3 = positions_bitSet(3);

        BitSet[] bitA = singleShipPos_bit2.get(3);
        BitSet[] bitB = singleShipPos_bit3.get(24);
        nicePrint(bitA);
        nicePrint(bitB);

        // System.out.println("Intersection? "+intersection(bitA, bitB));

        // List<BitSet[]> allMergedBoards = new ArrayList<>();

        // for (int i = 0; i < singleShipPos_bit2.size(); i++) {
        //     BitSet[] bitA = singleShipPos_bit2.get(i);

        //     for (int j = 0; j < singleShipPos_bit3.size(); j++) {
        //         BitSet[] bitB = singleShipPos_bit2.get(j);
        //         if (!intersection(bitA, bitB)) {
        //             allMergedBoards.add(mergePosition(bitA, bitB));
        //         }
        //     }
        // }
        // System.out.println("Merge count "+allMergedBoards.size()+"/"+totalPos);
        
        // BitSet[] bitAB = allMergedBoards.get(6);
        // nicePrint(bitAB);

    }
}
