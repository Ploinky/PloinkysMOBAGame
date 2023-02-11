import React, { useState } from 'react';
import { useNavigate } from 'react-router-dom';
import { NativeCaller } from '../../utils/NativeCaller';
import { Button } from '../Button/Button';
import { FlexContainer } from '../FlexContainer/FlexContainer';

import styles from './BaseLayout.module.scss';

export interface BaseLayoutProps {
    children: any;
};

function BaseLayout({ children }: BaseLayoutProps) {
    const [isQuitDialogOpen, setQuitDialogOpen] = useState(false);
    const navigate = useNavigate();

    return (
        <FlexContainer direction="column" className={styles.mainContainer}>
            <FlexContainer className={styles.header} id="#header" direction="row" align="center" justify="space-between">
                <h3>Ploinky's MOBA Game</h3>
                <FlexContainer direction="row" gap="1rem">
                    <Button className={styles.button} onClick={() => NativeCaller.quit()}>O</Button>
                    <Button className={styles.button} onClick={() => NativeCaller.minimize()}>-</Button>
                    <Button className={styles.button} onClick={() => setQuitDialogOpen(true)}>X</Button>
                </FlexContainer>
            </FlexContainer>
            <FlexContainer direction="column" className={styles.content}>
                {children}
            </FlexContainer>
            <dialog className={styles.quitDialog} open={isQuitDialogOpen}>
                <FlexContainer direction="column" gap="1rem">
                    <h5>Quit?</h5>
                    <Button className={styles.button} onClick={() => { navigate('/'); setQuitDialogOpen(false); }}>Logout</Button>
                    <Button className={styles.button} onClick={() => NativeCaller.quit()}>Quit</Button>
                    <Button className={styles.button} onClick={() => setQuitDialogOpen(false)}>Cancel</Button>
                </FlexContainer>
            </dialog>
        </FlexContainer>
    )
}

export { BaseLayout };
