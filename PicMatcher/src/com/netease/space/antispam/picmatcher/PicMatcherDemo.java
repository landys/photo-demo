package com.netease.space.antispam.picmatcher;

public class PicMatcherDemo {

    /**
     * @param args
     */
    public static void main(String[] args) {
        Thread thread = new Thread(new Runnable(){
            public void run() {
                PicMatcherImpl matcher = new PicMatcherImpl("./data");
                // set up index
                System.out.println("begin setup index");
                matcher.setupIndex("E:\\testpics\\heads");
                System.out.println("end setup index");
            }});
        thread.start();
        System.out.println("thread start");
        try {
            thread.join();
        } catch (InterruptedException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
        System.out.println("thread end");
    }

}
