import java.util.BitSet;

public class PrimitiveBoard{
    private BitSet bitBoard;
    boolean isBad;

    public PrimitiveBoard(Byte[] shipCodes, Ship[] fleet){
        bitBoard = new BitSet(Board.getLength()*Board.getLength()); // clone later(?
        isBad=false;

        for (int i = 0; i < shipCodes.length; i++) {             
            //decoding each byte
            byte encodedShip = shipCodes[i];
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

                // checks each position and if it is currently ocupied, return else put the ship there
                for (int j = 0; j < fleet[i].getLength(); j++) { 
                    if(dir) position=codedCoord+j*10;
                    else    position=codedCoord+j;

                    if (bitBoard.get(position)){
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
    }   

    private int[] makeIndexArray(Ship ship, int pos, boolean direction){ // direction horizontal (x) = true
        int[] positions = new int[ship.getLength()];

        for (int i = 0; i < ship.getLength(); i++) { 
            if(direction) positions[i]=pos+i*10;
            else          positions[i]=pos+i;
        }
        return positions;
    }

    public boolean getIsBad(){
        return isBad;
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