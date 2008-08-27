package com.picMatcher;

public class PicMatcherDemo {

    /**
     * Demo.
     * 
     * @param args
     */
    public static void main(String[] args) {
        PicMatcherImpl matcher = new PicMatcherImpl();
        // set up index
        matcher.setupIndex("E:\\testpics\\heads");

        // query
        System.out.println(matcher
                .query("E:\\testpics\\heads\\199565758487940524.jpg"));
        System.out.println(matcher
                .query("E:\\testpics\\mm270k\\AGRICLT2\\AH267.jpg"));
    }

}
