import java.util.ArrayList;
import java.util.List;

public class Board {
    // Needs a size (static)
    // list of ships
    // a 2d array?
    private static int BOARD_LENGTH;
    private boolean[][] grid; // [y][x]
    private List<Ship> ships;
    // maybe i should do it like J1-3 where its either null or a ship 
    // (i'd rather not make a cell but that doesnt seem half bad anymore)


    public Board(int boardLength) {
        Board.BOARD_LENGTH = boardLength; // revisit when thinking abt gridding
        grid = new boolean[BOARD_LENGTH][BOARD_LENGTH];
        this.ships = new ArrayList<>();
    }



    // addShip method - Checks for intersection, only adds if false
    public boolean addShip(Ship ship) {
        // check for intersection
        // add ship

        int len = ship.getLength();
        int sX=ship.getStartPosX();
        int sY=ship.getStartPosY();
        boolean dir =ship.getDirection();

        boolean intersection = false;

        if (dir){
            for (int i = sX; i < sX+len; i++) {
                if(grid[sY][i]){
                    intersection=true;
                }
            }
        }
        else{
            for (int i = sY; i < sY+len; i++) {
                if(grid[i][sX]){
                    intersection=true;
                }
            }
        }



        if(intersection){
            return false;
        }
        
        // no intersection -> add the ship
        ships.add(ship);
    
        
        if (dir){
            for (int i = sX; i < sX+len; i++) {
                grid[sY][i]=true;
            }
        }
        else{
            for (int i = sY; i < sY+len; i++) {
                grid[i][sX]=true;
            }
        }

        return true;
    }

    
    @Override
    public String toString(){
        String stringGrid="";
        
        for (boolean[] gridRow : grid) {
            stringGrid+="[ ";
            for (boolean cellValue : gridRow) {
                char symbol ='.';
                if(cellValue){
                    symbol='X';
                }
                stringGrid+=symbol+" ";
                
            }
            stringGrid+="]\n";
        }


        return stringGrid;
    }

}
