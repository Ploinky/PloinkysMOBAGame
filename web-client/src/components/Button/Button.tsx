import classNames from 'classnames';
import React from 'react';
import styles from './Button.module.scss';

export interface ButtonProps {
    children?: any;
    className?: any;
    onClick?: () => void;
    type?: "button" | "submit"
};

function Button({ children, className, onClick, type = "button" }: ButtonProps) {
    const classes = classNames(className, {
        [styles.button]: true,
    });

    return (
        <button className={classes} onClick={onClick} type={type}>{children}</button>
    )
}

export { Button };