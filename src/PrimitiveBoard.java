import java.util.BitSet;

public class PrimitiveBoard{
    private BitSet bitBoard;
    boolean isBad;

    public PrimitiveBoard(byte[] shipCodes, Ship[] fleet){
        bitBoard = new BitSet(Board.getLength()*Board.getLength()); // clone later(?
        isBad=false;
        int shipCounts=0;

        for (int i = 0; i < shipCodes.length; i++) {             
            //decoding each byte
            byte encodedShip = shipCodes[i];
            boolean dir = (encodedShip % 2 != 0); // if odd, then true (ie horizontal)
                
            int codedCoord = encodedShip & 0xff; // unsign it
            codedCoord>>=1; // get rid of directional info

            int x= (int) Math.floor(codedCoord/10); // undoes encoding in the form of x*10+y
            int y= codedCoord % 10;
            
            // char sillySymb = dir ? '→' : '↓';

            boolean itFitsOnTheBoard = (dir && x < (Board.getLength() - fleet[i].getLength()+1) && y < Board.getLength()) || 
                                      (!dir && y < (Board.getLength() - fleet[i].getLength()+1) && x < Board.getLength());
            
            if(itFitsOnTheBoard){
                // get all the indexes for the bitset
                int[] positions = new int[fleet[i].getLength()];

                for (int j = 0; j < fleet[i].getLength(); j++) { 
                    if(dir) positions[j]=codedCoord+j*10;
                    else    positions[j]=codedCoord+j;
                }

                // check each index (if 0, place ship, else intersection & return)
                for(int place: positions){
                    if (bitBoard.get(place)){
                        isBad=true;
                        return;
                    }
                    bitBoard.set(place);
                }

                shipCounts+=fleet[i].getLength();
            } else{
                isBad=true;
                return;
            }
        }
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