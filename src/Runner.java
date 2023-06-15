public class Runner {
    public static void main(String[] args) {
        Board b = new Board();
        // System.out.println(b.displaySetup());
        Ship s = new Ship(3,'3');
        try {
            b.placeShip(s, "(1,0)", false);
        } catch (Exception e) {
            // TODO: handle exception
            System.err.println("A bad thing"+e.getMessage()+"\n"+e.getStackTrace());
        }
        System.out.println(b.toString());

    }
}
