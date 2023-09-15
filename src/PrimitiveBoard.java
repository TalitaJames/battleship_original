import java.util.BitSet;
import java.util.Map;
import java.util.HashMap;

public class PrimitiveBoard{
    private BitSet bitBoard;
    boolean isBad;

    public PrimitiveBoard(Byte[] shipCodes, Ship[] fleet){
        this(shipCodes, fleet, new HashMap<Integer,Boolean>()); 
    }

    public PrimitiveBoard(Byte[] shipCodes, Ship[] fleet, Map<Integer,Boolean> hitmask){
        bitBoard = new BitSet(Board.getLength()*Board.getLength());
        isBad=false;

        // for each ship, check if it fits on the board, and try and place it
        for (int i = 0; i < shipCodes.length; i++) {             
            //decoding each Byte
            Byte encodedShip = shipCodes[i];
            boolean dir = (encodedShip % 2 != 0); // if odd, then true (ie horizontal)
                
            int codedCoord = encodedShip & 0xff; // unsign it
            codedCoord>>=1; // get rid of directional info

            int x= (int) Math.floor(codedCoord/10); // undoes encoding in the form of x*10+y
            int y= codedCoord % 10;
            
            // char sillySymb = dir ? '→' : '↓';

            // is the ship within board bounds?
            boolean itFitsOnTheBoard = (dir && x < (Board.getLength() - fleet[i].getLength()+1) && y < Board.getLength()) || 
                                      (!dir && y < (Board.getLength() - fleet[i].getLength()+1) && x < Board.getLength());
            
            if(itFitsOnTheBoard){
                int position;

                // checks each position and if it is currently ocupied, return, else put the ship there
                for (int j = 0; j < fleet[i].getLength(); j++) { 
                    if(dir) position=codedCoord+j*10;
                    else    position=codedCoord+j;
                    
                    // should the ship be in that position?
                    // (based on if there is a ship already there (bitBoard) and if it matches the hitmask)
                    boolean makeShip = !bitBoard.get(position) && ((hitmask.get(position)==null) || !(hitmask.get(position)==null) && hitmask.get(position));

                    if (!makeShip){  
                        isBad=true;
                        return;
                    }
                    bitBoard.set(position);
                }
            } else{
                isBad=true;
                return;
            }
        }

        // now all the ships are in place, check there aren't any X's in the hitmask that don't match the board
        for (Integer position : hitmask.keySet() ){
            if(hitmask.get(position) && !bitBoard.get(position)){
                isBad=true;
                return;
            }
        }        

    }   

    public boolean getIsBad(){
        return isBad;
    }

    public boolean[][] returnHeatmap(){
        boolean[][] singleHeatmap = new boolean[Board.getLength()][Board.getLength()];

        for (int y = 0; y < Board.getLength(); y++) {
            for (int x = 0; x < Board.getLength(); x++) {
                singleHeatmap[y][x] = bitBoard.get(x*10+y);
            }
        }

        return singleHeatmap;
    }

    @Override
    public String toString(){
        String grid = "";

        grid+= isBad+" bitBoard\n";
        grid+= bitBoard+"\n";
        for (int y = 0; y < Board.getLength(); y++) {
            grid +="[";
            for (int x = 0; x < Board.getLength(); x++) {
                char rep = bitBoard.get(x*10+y) ? 'X' : '.';
                grid += rep + " ";
            }
            grid +="]\n";
        }
        return grid;
    }
}