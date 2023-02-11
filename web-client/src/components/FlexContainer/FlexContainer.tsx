import React from 'react';
import classNames from 'classnames';
import styles from './FlexContainer.module.scss';

export interface FlexContainerProps {
    direction?: 'row' | 'column';
    align?: 'flex-start' | 'flex-end' | 'space-between' | 'space-around' | 'center';
    justify?: 'flex-start' | 'flex-end' | 'space-between' | 'space-around' | 'center';
    gap?: string;
    className?: string;
    children: any;
    id?: string;
}

function FlexContainer({
    direction = 'row',
    align = 'center',
    justify = 'center',
    gap = "0",
    className,
    children,
    id,
}: FlexContainerProps) {
    const classes = classNames(className, {
        [styles.flexContainer]: true,
        [styles.row]: direction === 'row',
        [styles.column]: direction === 'column',
    });
    return (
        <div className={classes} id={id} style={{ alignItems: align, justifyContent: justify, gap: gap }}>
            {children}
        </div>
    )
}

export { FlexContainer };