package com.netease.space.antispam.picmatcher;

public class PicMatcherImplTest {

    /**
     * @param args
     */
    public static void main(String[] args) throws Exception {
        if (args.length < 2) {
            System.out
                    .println(">java -jar PicMatcher.jar img_file times interval(ms)");
            System.out
                    .println(">java -jar PicMatcher.jar \"E:\\testpics\\heads\\1.jpg\" 100 1000");
            return;
        }
        PicMatcherImpl matcher = new PicMatcherImpl("E:/projects/photodemo/codes/branches/TRY-refactor-jni-envelop/bin/Release/data");
        
        int times = Integer.parseInt(args[1]);
        int interval = Integer.parseInt(args[2]);
        for (int i=0; i<times; i++) {
            // query
            System.out.println(matcher.query(args[0]));
            Thread.sleep(interval);
        }
    }

}
