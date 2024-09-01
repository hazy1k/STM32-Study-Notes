#ifndef __RECORDER_H
#define	__RECORDER_H


typedef enum
{
	STATE_WAIT =0,
	STATE_RECORDING,
	STATE_PLAYING,
}enumRecorder;


void Recorder_Run(void);








#endif /* __RECORDER_H */
